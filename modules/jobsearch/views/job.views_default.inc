<?php
// $Id: job.views_default.inc,v 1.1.2.1 2010/01/13 15:35:40 kbahey Exp $

function job_views_default_views() {
  $views = array();

  $view = new view;
  $view->name = 'my_applications';
  $view->description = 'My applications: shows job applications by the currently logged in user.';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'job';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  /* Edit this to true to make a default view disabled initially */ $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('relationships', array( 'nid' => array( 'label' => 'Job Applied For', 'required' => 0, 'id' => 'nid', 'table' => 'job', 'field' => 'nid', 'relationship' => 'none', ), 'uid' => array( 'id' => 'uid', 'table' => 'job', 'field' => 'uid', ), ));
  $handler->override_option('fields', array( 'created' => array( 'label' => 'Post date', 'date_format' => 'custom', 'custom_date_format' => 'm/d/Y', 'exclude' => 0, 'id' => 'created', 'table' => 'node', 'field' => 'created', 'relationship' => 'nid', ), 'title' => array( 'label' => 'Title', 'link_to_node' => 1, 'exclude' => 0, 'id' => 'title', 'table' => 'node', 'field' => 'title', 'relationship' => 'nid', ), ));
  $handler->override_option('filters', array( 'uid_current' => array( 'operator' => '=', 'value' => '1', 'group' => '0', 'exposed' => FALSE, 'expose' => array( 'operator' => FALSE, 'label' => '', ), 'id' => 'uid_current', 'table' => 'users', 'field' => 'uid_current', 'relationship' => 'uid', ), ));
  $handler->override_option('access', array( 'type' => 'none', ));
  $handler->override_option('title', 'My Applications');
  $handler->override_option('empty', 'You have not applied for any jobs.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array( 'grouping' => '', 'override' => 1, 'sticky' => 0, 'order' => 'asc', 'columns' => array( 'created' => 'created', 'title' => 'title', ), 'info' => array( 'created' => array( 'sortable' => 1, 'separator' => '', ), 'title' => array( 'sortable' => 1, 'separator' => '', ), ), 'default' => '-1', ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'my_applications');
  $handler->override_option('menu', array( 'type' => 'none', 'title' => '', 'weight' => 0, 'name' => 'navigation', ));
  $handler->override_option('tab_options', array( 'type' => 'none', 'title' => '', 'weight' => 0, ));

  $views[$view->name] = $view;


  $view = new view;
  $view->name = 'job_applications';
  $view->description = 'Job applications: Recruiter/employer view of all jobs applied for.';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'job';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  /* Edit this to true to make a default view disabled initially */ $handler = $view->new_display('default', 'Job Applications', 'default');
  $handler->override_option('relationships', array( 'nid' => array( 'label' => 'Job Applied For', 'required' => 0, 'id' => 'nid', 'table' => 'job', 'field' => 'nid', 'relationship' => 'none', ), 'uid' => array( 'label' => 'Resume Applicant', 'required' => 0, 'id' => 'uid', 'table' => 'job', 'field' => 'uid', 'relationship' => 'none', ), 'resume_nid' => array( 'label' => 'Resume', 'required' => 0, 'id' => 'resume_nid', 'table' => 'job', 'field' => 'resume_nid', 'relationship' => 'none', ), 'uid_1' => array( 'label' => 'Job Poster', 'required' => 0, 'id' => 'uid_1', 'table' => 'node_revisions', 'field' => 'uid', 'relationship' => 'nid', ), ));
  $handler->override_option('fields', array( 'title' => array( 'label' => 'Job', 'link_to_node' => 1, 'exclude' => 0, 'id' => 'title', 'table' => 'node', 'field' => 'title', 'relationship' => 'nid', ), 'name' => array( 'label' => 'Recruiter', 'link_to_user' => 1, 'exclude' => 0, 'id' => 'name', 'table' => 'users', 'field' => 'name', 'relationship' => 'nid', ), 'name_1' => array( 'label' => 'Applicant', 'link_to_user' => 1, 'exclude' => 0, 'id' => 'name_1', 'table' => 'users', 'field' => 'name', 'relationship' => 'uid', ), 'timestamp' => array( 'label' => 'Date/Time', 'date_format' => 'small', 'custom_date_format' => '', 'exclude' => 0, 'id' => 'timestamp', 'table' => 'job', 'field' => 'timestamp', 'relationship' => 'none', ), ));
  $handler->override_option('filters', array( 'status' => array( 'operator' => '=', 'value' => array( 'value' => '1', 'min' => '', 'max' => '', ), 'group' => '0', 'exposed' => FALSE, 'expose' => array( 'operator' => FALSE, 'label' => '', ), 'id' => 'status', 'table' => 'job', 'field' => 'status', 'relationship' => 'none', ), ));
  $handler->override_option('access', array( 'type' => 'none', ));
  $handler->override_option('title', 'Job Applications');
  $handler->override_option('empty', 'There are no job applications.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array( 'grouping' => '', 'override' => 1, 'sticky' => 0, 'order' => 'asc', 'columns' => array( 'title' => 'title', 'name' => 'name', 'name_1' => 'name_1', 'timestamp' => 'timestamp', ), 'info' => array( 'title' => array( 'sortable' => 1, 'separator' => '', ), 'name' => array( 'sortable' => 1, 'separator' => '', ), 'name_1' => array( 'sortable' => 1, 'separator' => '', ), 'timestamp' => array( 'sortable' => 1, 'separator' => '', ), ), 'default' => '-1', ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'job_applications');
  $handler->override_option('menu', array( 'type' => 'none', 'title' => '', 'weight' => 0, 'name' => 'navigation', ));
  $handler->override_option('tab_options', array( 'type' => 'none', 'title' => '', 'weight' => 0, ));

  $views[$view->name] = $view;


  $view = new view;
  $view->name = 'seeker_applications';
  $view->description = 'Seeker applications: a view that accepts an optional UID argument to list all applications, or a specific user\'s applications';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'job';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE;
  /* Edit this to true to make a default view disabled initially */ $handler = $view->new_display('default', 'Seeker applications', 'default');
  $handler->override_option('relationships', array( 'nid' => array( 'label' => 'Job Applied For', 'required' => 0, 'id' => 'nid', 'table' => 'job', 'field' => 'nid', 'relationship' => 'none', ), 'uid' => array( 'label' => 'Resume Applicant', 'required' => 0, 'id' => 'uid', 'table' => 'job', 'field' => 'uid', 'relationship' => 'none', ), 'resume_nid' => array( 'label' => 'Resume', 'required' => 0, 'id' => 'resume_nid', 'table' => 'job', 'field' => 'resume_nid', 'relationship' => 'none', ), 'uid_1' => array( 'label' => 'Job Poster', 'required' => 0, 'id' => 'uid_1', 'table' => 'node_revisions', 'field' => 'uid', 'relationship' => 'nid', ), ));
  $handler->override_option('fields', array( 'title' => array( 'label' => 'Job', 'link_to_node' => 1, 'exclude' => 0, 'id' => 'title', 'table' => 'node', 'field' => 'title', 'relationship' => 'nid', ), 'name' => array( 'label' => 'Recruiter', 'link_to_user' => 1, 'exclude' => 0, 'id' => 'name', 'table' => 'users', 'field' => 'name', 'relationship' => 'nid', ), 'timestamp' => array( 'label' => 'Date/Time', 'date_format' => 'small', 'custom_date_format' => '', 'exclude' => 0, 'id' => 'timestamp', 'table' => 'job', 'field' => 'timestamp', 'relationship' => 'none', ), ));
  $handler->override_option('arguments', array( 'uid' => array( 'default_action' => 'ignore', 'style_plugin' => 'default_summary', 'style_options' => array(), 'wildcard' => 'all', 'wildcard_substitution' => 'All', 'title' => 'Applicant UID', 'default_argument_type' => 'fixed', 'default_argument' => '', 'validate_type' => 'none', 'validate_fail' => 'not found', 'break_phrase' => 0, 'not' => 0, 'id' => 'uid', 'table' => 'users', 'field' => 'uid', 'relationship' => 'uid', 'default_options_div_prefix' => '', 'default_argument_fixed' => '', 'default_argument_php' => '', 'validate_argument_node_type' => array( 'classifiedpost' => 0, 'education' => 0, 'employment' => 0, 'jobposting' => 0, 'page' => 0, 'resume' => 0, 'story' => 0, ), 'validate_argument_node_access' => 0, 'validate_argument_nid_type' => 'nid', 'validate_argument_vocabulary' => array( '4' => 0, '5' => 0, '9' => 0, '6' => 0, '3' => 0, '1' => 0, '8' => 0, ), 'validate_argument_type' => 'tid', 'validate_argument_php' => '', ), ));
  $handler->override_option('access', array( 'type' => 'none', ));
  $handler->override_option('title', 'Seeker applications');
  $handler->override_option('empty', 'There are no job applications.');
  $handler->override_option('empty_format', '2');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array( 'grouping' => '', 'override' => 1, 'sticky' => 0, 'order' => 'asc', 'columns' => array( 'title' => 'title', 'name' => 'name', 'name_1' => 'name_1', 'timestamp' => 'timestamp', ), 'info' => array( 'title' => array( 'sortable' => 1, 'separator' => '', ), 'name' => array( 'sortable' => 1, 'separator' => '', ), 'name_1' => array( 'sortable' => 1, 'separator' => '', ), 'timestamp' => array( 'sortable' => 1, 'separator' => '', ), ), 'default' => '-1', ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'seeker_applications');
  $handler->override_option('menu', array( 'type' => 'none', 'title' => '', 'weight' => 0, 'name' => 'navigation', ));
  $handler->override_option('tab_options', array( 'type' => 'none', 'title' => '', 'weight' => 0, ));

  $views[$view->name] = $view;

  return $views;

}