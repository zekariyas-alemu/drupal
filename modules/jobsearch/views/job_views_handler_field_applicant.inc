<?php
// $Id: job_views_handler_field_applicant.inc,v 1.1.2.1 2010/01/13 15:35:40 kbahey Exp $

/**
* Field handler for variable values
*/
class job_views_handler_field_applicant extends views_handler_field {
  function render($values) {
     $obj = new stdClass();
     $obj = user_load(array('uid' => $values->variable_value));
     return theme('username', $obj);
  }
}