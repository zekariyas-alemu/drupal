<?php
// $Id: job.views.inc,v 1.1.2.1 2010/01/13 15:35:40 kbahey Exp $

/**
 * Implementation of hook_views_tables():
 * Present fields and filters for user data.
 */
function job_views_data() {

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['job']['table']['group'] = t('Job');

  // Advertise this table as a possible base table
  $data['job']['table']['base'] = array(
    'field'    => 'nid',
    'title'    => t('Job'),
    'help'     => t("Jobs are job postings"),
    'weight'   => 10,
  );

  // For other base tables, explain how we join -
  $data['job']['table']['join'] = array(
      'node'   => array(
         'left_field'   => 'nid',
         'field'        => 'nid',
      ),
      'users'   => array(
         'left_field'   => 'uid',
         'field'        => 'uid',
      ),
  );

  //   [TABLE_NAME][COLUMN]
  $data['job']['nid'] = array(
    'title'       => t('Job Node ID'),
    'help'        => t('Node ID of the job node.'),
    // Information for displaying the value
    'field'       => array(
      'handler'   => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    // Information for sorting on a value.
    'sort'        => array(
      'handler'   => 'views_handler_sort',
    ),
    // Information for accepting a value as a filter
    'filter'      => array(
      'handler'   => 'views_handler_filter_string',
    ),
    // Information for accepting a value as an argument
    'argument'    => array(
      'handler'   => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Job Applied For'),
    ),
  );

  //   [TABLE_NAME][COLUMN]
  $data['job']['timestamp'] = array(
    'title'       => t('Date/Time Job applied for'),
    'help'        => t('Date/time applied for this Job.'),
    // Information for displaying the value
    'field'       => array(
      'handler'   => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    // Information for accepting a value as a filter
    'filter'      => array(
      'handler'   => 'views_handler_filter_date',
    ),
    // Information for sorting on a value.
    'sort'        => array(
      'handler'   => 'views_handler_sort',
    ),
    // Information for accepting a value as an argument
    'argument'    => array(
      'handler'   => 'views_handler_argument_date',
    ),
  );

  //   [TABLE_NAME][COLUMN]
   $data['job']['resume_nid'] = array(
    'title'       => t('Resume Node ID'),
    'help'        => t('Node ID of the resume node.'),
    // Information for displaying the value
    'field'       => array(
      'handler'   => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    // Information for accepting a value as a filter
    'filter'      => array(
      'handler'   => 'views_handler_filter_numeric',
    ),
    // Information for sorting on a value.
    'sort'        => array(
      'handler'   => 'views_handler_sort',
    ),
    // Information for accepting a value as an argument
    'argument'    => array(
      'handler'   => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'base' => 'node',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Resume'),
    ),
  );

  //   [TABLE_NAME][COLUMN]
  $data['job']['uid'] = array(
    'title'       => t('Resume Applicant ID'),
    'help'        => t('ID of the applicant user.'),
    // Information for displaying the value
    'field'       => array(
      'handler'   => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    // Information for accepting a value as a filter
    'filter'      => array(
      'handler'   => 'views_handler_filter_numeric',
    ),
    // Information for sorting on a value.
    'sort'        => array(
      'handler'   => 'views_handler_sort',
    ),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Resume Applicant'),
    ),
  );

  //   [TABLE_NAME][COLUMN]
  $data['job']['status'] = array(
    'title'       => t('Job Status'),
    'help'        => t('Status of the Job posting.'),
    // Information for displaying the value
    'field'       => array(
      'handler'   => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    // Information for accepting a value as a filter
    'filter'      => array(
      'handler'   => 'views_handler_filter_numeric',
    ),
    // Information for sorting on a value.
    'sort'        => array(
      'handler'   => 'views_handler_sort',
    ),
  );

  return $data;
}


function job_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'job') .'/views',
    ),
    'handlers' => array(
      // field handlers
      'job_views_handler_field_applicant' => array(
        'parent' => 'views_handler_field',
      ),
    )
  );
}

