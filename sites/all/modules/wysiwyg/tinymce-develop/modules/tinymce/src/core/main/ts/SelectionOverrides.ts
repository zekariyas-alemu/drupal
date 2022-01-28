/**
 * Copyright (c) Tiny Technologies, Inc. All rights reserved.
 * Licensed under the LGPL or a commercial license.
 * For LGPL see License.txt in the project root for license information.
 * For commercial licenses see https://www.tiny.cloud/
 */

import { Arr, Obj, Unicode } from '@ephox/katamari';
import { Attribute, Compare, Css, Focus, Insert, InsertAll, Remove, SelectorFilter, SelectorFind, SugarElement } from '@ephox/sugar';

import Editor from './api/Editor';
import VK from './api/util/VK';
import * as CaretContainer from './caret/CaretContainer';
import * as CaretUtils from './caret/CaretUtils';
import * as ClosestCaretCandidate from './caret/ClosestCaretCandidate';
import { FakeCaret, isFakeCaretTarget } from './caret/FakeCaret';
import * as FakeCaretUtils from './caret/FakeCaretUtils';
import * as CefUtils from './dom/CefUtils';
import * as NodeType from './dom/NodeType';
import * as DragDropOverrides from './DragDropOverrides';
import * as EditorView from './EditorView';
import * as CefFocus from './focus/CefFocus';
import * as EditorFocus from './focus/EditorFocus';
import * as MediaFocus from './focus/MediaFocus';
import * as Rtc from './Rtc';

const isContentEditableFalse = NodeType.isContentEditableFalse;

interface SelectionOverrides {
  showCaret: (direction: number, node: Element, before: boolean, scrollIntoView?: boolean) => Range | null;
  showBlockCaretContainer: (blockCaretContainer: Element) => void;
  hideFakeCaret: () => void;
  destroy: () => void;
}

const getContentEditableRoot = (editor: Editor, node: Node) => CefUtils.getContentEditableRoot(editor.getBody(), node);

const SelectionOverrides = (editor: Editor): SelectionOverrides => {
  const selection = editor.selection, dom = editor.dom;
  const isBlock = dom.isBlock as (node: Node) => node is HTMLElement;

  const rootNode = editor.getBody();
  const fakeCaret = FakeCaret(editor, rootNode, isBlock, () => EditorFocus.hasFocus(editor));
  const realSelectionId = 'sel-' + dom.uniqueId();
  const elementSelectionAttr = 'data-mce-selected';
  let selectedElement;

  const isFakeSelectionElement = (node: Node) => dom.hasClass(node, 'mce-offscreen-selection');
  // Note: isChildOf will return true if node === rootNode, so we need an additional check for that
  const isFakeSelectionTargetElement = (node: Node): node is HTMLElement =>
    node !== rootNode && (isContentEditableFalse(node) || NodeType.isMedia(node)) && dom.isChildOf(node, rootNode);

  const getRealSelectionElement = () => {
    const container = dom.get(realSelectionId);
    return container ? container.getElementsByTagName('*')[0] as HTMLElement : container;
  };

  const setRange = (range: Range | null) => {
    if (range) {
      selection.setRng(range);
    }
  };

  const showCaret = (direction: number, node: HTMLElement, before: boolean, scrollIntoView: boolean = true): Range => {
    const e = editor.fire('ShowCaret', {
      target: node,
      direction,
      before
    });

    if (e.isDefaultPrevented()) {
      return null;
    }

    if (scrollIntoView) {
      selection.scrollIntoView(node, direction === -1);
    }

    return fakeCaret.show(before, node);
  };

  const showBlockCaretContainer = (blockCaretContainer: HTMLElement) => {
    if (blockCaretContainer.hasAttribute('data-mce-caret')) {
      CaretContainer.showCaretContainerBlock(blockCaretContainer);
      selection.scrollIntoView(blockCaretContainer);
    }
  };

  const registerEvents = () => {
    editor.on('click', (e) => {
      const contentEditableRoot = getContentEditableRoot(editor, e.target);
      if (contentEditableRoot) {
        // Prevent clicks on links in a cE=false element
        if (isContentEditableFalse(contentEditableRoot)) {
          e.preventDefault();
          editor.focus();
        }
      }
    });

    editor.on('blur NewBlock', removeElementSelection);

    editor.on('ResizeWindow FullscreenStateChanged', fakeCaret.reposition);

    editor.on('tap', (e) => {
      const targetElm = e.target;
      const contentEditableRoot = getContentEditableRoot(editor, targetElm);
      if (isContentEditableFalse(contentEditableRoot)) {
        e.preventDefault();
        FakeCaretUtils.selectNode(editor, contentEditableRoot).each(setElementSelection);
      } else if (isFakeSelectionTargetElement(targetElm)) {
        FakeCaretUtils.selectNode(editor, targetElm).each(setElementSelection);
      }
    }, true);

    editor.on('mousedown', (e: MouseEvent) => {
      const targetElm = e.target as Element;

      if (targetElm !== rootNode && targetElm.nodeName !== 'HTML' && !dom.isChildOf(targetElm, rootNode)) {
        return;
      }

      if (EditorView.isXYInContentArea(editor, e.clientX, e.clientY) === false) {
        return;
      }

      // Remove needs to be called here since the mousedown might alter the selection without calling selection.setRng
      // and therefore not fire the AfterSetSelectionRange event.
      removeElementSelection();
      hideFakeCaret();

      const closestContentEditable = getContentEditableRoot(editor, targetElm);
      if (isContentEditableFalse(closestContentEditable)) {
        e.preventDefault();
        FakeCaretUtils.selectNode(editor, closestContentEditable).each(setElementSelection);
      } else {
        ClosestCaretCandidate.closestFakeCaretCandidate(rootNode, e.clientX, e.clientY).each((caretInfo) => {
          e.preventDefault();
          const range = showCaret(1, caretInfo.node as HTMLElement, caretInfo.position === ClosestCaretCandidate.FakeCaretPosition.Before, false);
          setRange(range);

          // Set the focus after the range has been set to avoid potential issues where the body has no selection
          if (NodeType.isElement(closestContentEditable)) {
            closestContentEditable.focus();
          } else {
            editor.getBody().focus();
          }
        });
      }
    });

    editor.on('keypress', (e) => {
      if (VK.modifierPressed(e)) {
        return;
      }

      if (isContentEditableFalse(selection.getNode())) {
        e.preventDefault();
      }
    });

    editor.on('GetSelectionRange', (e) => {
      let rng = e.range;

      if (selectedElement) {
        if (!selectedElement.parentNode) {
          selectedElement = null;
          return;
        }

        rng = rng.cloneRange();
        rng.selectNode(selectedElement);
        e.range = rng;
      }
    });

    editor.on('SetSelectionRange', (e) => {
      // If the range is set inside a short ended element, then move it
      // to the side as IE for example will try to add content inside
      e.range = normalizeVoidElementSelection(e.range);

      const rng = setElementSelection(e.range, e.forward);
      if (rng) {
        e.range = rng;
      }
    });

    const isPasteBin = (node: Element): boolean => node.id === 'mcepastebin';

    editor.on('AfterSetSelectionRange', (e) => {
      const rng = e.range;
      const parentNode = rng.startContainer.parentNode;

      if (!isRangeInCaretContainer(rng) && !isPasteBin(parentNode as Element)) {
        hideFakeCaret();
      }

      if (!isFakeSelectionElement(parentNode)) {
        removeElementSelection();
      }
    });

    editor.on('copy', (e) => {
      const clipboardData = e.clipboardData;

      // Make sure we get proper html/text for the fake cE=false selection
      if (!e.isDefaultPrevented() && e.clipboardData) {
        const realSelectionElement = getRealSelectionElement();
        if (realSelectionElement) {
          e.preventDefault();
          clipboardData.clearData();
          clipboardData.setData('text/html', realSelectionElement.outerHTML);
          // outerText is a nonstandard property and doesn't exist on Firefox, so fallback to innerText
          clipboardData.setData('text/plain', (realSelectionElement as any).outerText || realSelectionElement.innerText);
        }
      }
    });

    DragDropOverrides.init(editor);
    CefFocus.setup(editor);
    MediaFocus.setup(editor);
  };

  const isWithinCaretContainer = (node: Node) => (
    CaretContainer.isCaretContainer(node) ||
    CaretContainer.startsWithCaretContainer(node) ||
    CaretContainer.endsWithCaretContainer(node)
  );

  const isRangeInCaretContainer = (rng: Range) =>
    isWithinCaretContainer(rng.startContainer) || isWithinCaretContainer(rng.endContainer);

  const normalizeVoidElementSelection = (rng: Range) => {
    const voidElements = editor.schema.getVoidElements();
    const newRng = dom.createRng();
    const startContainer = rng.startContainer;
    const startOffset = rng.startOffset;
    const endContainer = rng.endContainer;
    const endOffset = rng.endOffset;

    if (Obj.has(voidElements, startContainer.nodeName.toLowerCase())) {
      if (startOffset === 0) {
        newRng.setStartBefore(startContainer);
      } else {
        newRng.setStartAfter(startContainer);
      }
    } else {
      newRng.setStart(startContainer, startOffset);
    }

    if (Obj.has(voidElements, endContainer.nodeName.toLowerCase())) {
      if (endOffset === 0) {
        newRng.setEndBefore(endContainer);
      } else {
        newRng.setEndAfter(endContainer);
      }
    } else {
      newRng.setEnd(endContainer, endOffset);
    }

    return newRng;
  };

  const setupOffscreenSelection = (node: Element, targetClone: Node) => {
    const body = SugarElement.fromDom(editor.getBody());
    const doc = editor.getDoc();
    const realSelectionContainer = SelectorFind.descendant<HTMLElement>(body, '#' + realSelectionId).getOrThunk(() => {
      const newContainer = SugarElement.fromHtml<HTMLDivElement>('<div data-mce-bogus="all" class="mce-offscreen-selection"></div>', doc);
      Attribute.set(newContainer, 'id', realSelectionId);
      Insert.append(body, newContainer);
      return newContainer;
    });

    const newRange = dom.createRng();
    Remove.empty(realSelectionContainer);
    InsertAll.append(realSelectionContainer, [
      SugarElement.fromText(Unicode.nbsp, doc),
      SugarElement.fromDom(targetClone),
      SugarElement.fromText(Unicode.nbsp, doc)
    ]);
    newRange.setStart(realSelectionContainer.dom.firstChild, 1);
    newRange.setEnd(realSelectionContainer.dom.lastChild, 0);

    Css.setAll(realSelectionContainer, {
      top: dom.getPos(node, editor.getBody()).y + 'px'
    });

    Focus.focus(realSelectionContainer);
    const sel = selection.getSel();
    sel.removeAllRanges();
    sel.addRange(newRange);

    return newRange;
  };

  const selectElement = (elm: HTMLElement) => {
    const targetClone = elm.cloneNode(true);
    const e = editor.fire('ObjectSelected', { target: elm, targetClone });
    if (e.isDefaultPrevented()) {
      return null;
    }

    // Setup the offscreen selection
    const range = setupOffscreenSelection(elm, e.targetClone);

    // We used to just remove all data-mce-selected values and set 1 on node.
    // But data-mce-selected can be values other than 1 so keep existing value if
    // node has one, and remove data-mce-selected from everything else
    const nodeElm = SugarElement.fromDom(elm);
    Arr.each(SelectorFilter.descendants(SugarElement.fromDom(editor.getBody()), '*[data-mce-selected]'), (elm) => {
      if (!Compare.eq(nodeElm, elm)) {
        Attribute.remove(elm, elementSelectionAttr);
      }
    });

    if (!dom.getAttrib(elm, elementSelectionAttr)) {
      elm.setAttribute(elementSelectionAttr, '1');
    }
    selectedElement = elm;
    hideFakeCaret();

    return range;
  };

  const setElementSelection = (range: Range, forward?: boolean) => {
    if (!range) {
      return null;
    }

    if (range.collapsed) {
      if (!isRangeInCaretContainer(range)) {
        const dir = forward ? 1 : -1;
        const caretPosition = CaretUtils.getNormalizedRangeEndPoint(dir, rootNode, range);

        const beforeNode = caretPosition.getNode(!forward);
        if (isFakeCaretTarget(beforeNode)) {
          return showCaret(dir, beforeNode, forward ? !caretPosition.isAtEnd() : false, false);
        }

        const afterNode = caretPosition.getNode(forward);
        if (isFakeCaretTarget(afterNode)) {
          return showCaret(dir, afterNode, forward ? false : !caretPosition.isAtEnd(), false);
        }
      }

      return null;
    }

    let startContainer = range.startContainer;
    let startOffset = range.startOffset;
    const endOffset = range.endOffset;

    // Normalizes <span cE=false>[</span>] to [<span cE=false></span>]
    if (startContainer.nodeType === 3 && startOffset === 0 && isContentEditableFalse(startContainer.parentNode)) {
      startContainer = startContainer.parentNode;
      startOffset = dom.nodeIndex(startContainer);
      startContainer = startContainer.parentNode;
    }

    if (startContainer.nodeType !== 1) {
      return null;
    }

    if (endOffset === startOffset + 1 && startContainer === range.endContainer) {
      const node = startContainer.childNodes[startOffset];

      if (isFakeSelectionTargetElement(node)) {
        return selectElement(node);
      }
    }

    return null;
  };

  const removeElementSelection = () => {
    if (selectedElement) {
      selectedElement.removeAttribute(elementSelectionAttr);
    }
    SelectorFind.descendant(SugarElement.fromDom(editor.getBody()), '#' + realSelectionId).each(Remove.remove);
    selectedElement = null;
  };

  const destroy = () => {
    fakeCaret.destroy();
    selectedElement = null;
  };

  const hideFakeCaret = () => {
    fakeCaret.hide();
  };

  if (!Rtc.isRtc(editor)) {
    registerEvents();
  }

  return {
    showCaret,
    showBlockCaretContainer,
    hideFakeCaret,
    destroy
  };
};

export default SelectionOverrides;