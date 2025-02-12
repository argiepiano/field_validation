<?php
/**
 * @file
 * Manages validation rules administration UI.
 */

/**
 * Menu callback function using to dispatch.
 */
function field_validation_ui_callback_dispatch($instance, $arg1 = NULL, $arg2 = NULL, $arg3 = NULL) {
  $output = '';
  if (empty($arg1)) {
    backdrop_set_title(t('Validation'));
    $output .= field_validation_ui_manage($instance);
  }
  elseif ($arg1 == 'add') {
    backdrop_set_title(t('Add validation'));
    $form = backdrop_get_form('field_validation_ui_manage_rule', $instance, 'add', $arg2);
    $output .= backdrop_render($form);
  }
  elseif ($arg1 == 'edit') {
    backdrop_set_title(t('Edit rule'));
    $form = backdrop_get_form('field_validation_ui_manage_rule', $instance, 'edit', $arg2, $arg3);
    $output .= backdrop_render($form);
  }
  elseif ($arg1 == 'delete') {
    backdrop_set_title(t('Delete rule'));
    $form = backdrop_get_form('field_validation_ui_delete_rule', $arg2);
    $output .= backdrop_render($form);
  }
  elseif ($arg1 == 'overwrite') {
    backdrop_set_title(t('Overwrite rule'));
    $form = backdrop_get_form('field_validation_ui_manage_rule', $instance, 'edit', $arg2, $arg3);
    $output .= backdrop_render($form);
  }
  else {
    backdrop_set_title(t('Validation'));
    $output .= field_validation_manage($instance);
  }

  return $output;
}

/**
 * Menu callback function to show an overview of the existing validation rules, and the option to add a rule.
 */
function field_validation_ui_manage($instance) {
  $output = '';
  // $rules = field_validation_get_field_rules($instance);
  $bundle = $instance['bundle'];
  $entity_type = $instance['entity_type'];
  $field_name = $instance['field_name'];

  // Switch from using ctools exports to config.
  //ctools_include('export');
  //$rules = ctools_export_load_object('field_validation_rule', 'conditions', array('entity_type' => $entity_type, 'bundle' => $bundle, 'field_name' => $field_name));

  $prefix = 'field_validation.rule.' . $entity_type . '.' . $bundle . '.' . $field_name;
  $names = config_get_names_with_prefix($prefix);
  $rules = config_load_multiple($names);

  $output .= theme('field_validation_ui_manage_overview', array('rules' => $rules, 'instance' => $instance));
  $output .= theme('field_validation_ui_manage_add_rule', array('instance' => $instance));
  return $output;
}


/**
 * Callback function to add or edit a validation rule.
 */
function field_validation_ui_manage_rule($form, $form_state, $instance, $action = 'add', $validator = 'field_validation_regex_validator', $machine = FALSE) {
  // $form = array();
  // $rule_validator = field_validation_get_validator_info($validator);
  $field_info =  $instance['entity_type'] . '.' . $instance['bundle'] . '.' . $instance['field_name'];
  if ($machine) {
    // Retrieve the existing rule.
    $config_name = 'field_validation.rule.' . $field_info . '.' . $machine;
    $rule = config($config_name);
    $form_state['item'] = $rule->get();

    // Set default values.
    $rulename = $rule->get('rulename');
    $col = $rule->get('col');
    $error_message = $rule->get('error_message');
  }

  $form['validator'] = array(
    '#type' => 'hidden',
    '#value' => $validator,
  );

  $form['action'] = array(
    '#type' => 'hidden',
    '#value' => $action,
  );

  $form['entity_type'] = array(
    '#type' => 'hidden',
    '#value' => $instance['entity_type'],
  );

  $form['bundle'] = array(
    '#type' => 'hidden',
    '#value' => $instance['bundle'],
  );

  $form['field_name'] = array(
    '#type' => 'hidden',
    '#value' => $instance['field_name'],
  );

  $form['rulename'] = array(
    '#type' => 'textfield',
    '#title' => t('Rule name'),
    '#default_value' => isset($rulename) ? $rulename : '',
    '#required' => TRUE,
    '#size' => 60,
    '#maxlength' => 255,
    '#weight' => 1,
  );

  $form['machine'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($machine) ? $machine : '',
    '#machine_name' => array(
      'exists' => '_field_validation_ui_rule_name_exists',
      'source' => array('rulename'),
    ),
    '#weight' => 2,
    '#maxlength' => 32,
  );

  $field = field_info_field($instance['field_name']);
  $col_options = array(
    '' => t('Choose a column'),
  );
  $columns = !empty($field['columns']) ? $field['columns'] : array();
  foreach ($columns as $key => $column) {
    $col_options[$key] = $key;
  }

  // Support free tagging.
  if ($field['type'] == 'taxonomy_term_reference') {
    $col_options['machine'] = 'machine';
  }

  $form['col'] = array(
    '#type' => 'select',
    '#options' => $col_options,
    '#title' => t('Column'),
    '#description' => t('A column defined in the hook_field_schema() of this field.'),
    '#default_value' => isset($col) ? $col : '',
    '#required' => TRUE,
    '#weight' => 3,
  );

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#weight' => 4,
  );

  // @todo switch from using ctools plugins to normal classes.
  //ctools_include('plugins');
  //$plugin = ctools_get_plugins('field_validation', 'validator', $validator);
  //$class = ctools_plugin_get_class($plugin, 'handler');

  $info = field_validation_get_validator_info($validator);
  $class = $info['class'];
  $entity_type = $instance['entity_type'];
  $validator_class = new $class($entity_type);
  $validator_class->settings_form($form, $form_state);

  $form['error_message'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom error message'),
    '#description' => t("Specify an error message that should be displayed when user input doesn't pass validation"),
    '#required' => TRUE,
    '#size' => 60,
    '#maxlength' => 255,
    '#default_value' => isset($error_message) ? $error_message : '',
    '#weight' => 5,
  );

  $output = '<p>' . t('The following tokens are available for error message.' . '</p>');
  $token_help = $validator_class->token_help();
  if (!empty($token_help)) {
    $items = array();
    foreach ($token_help as $key => $value) {
      $items[] = $key . ' == ' . $value;
    }
    $output .= theme('item_list', array('items' => $items));
  }

  $form['token_help'] = array(
    '#type' => 'fieldset',
    '#title' => t('Replacement patterns'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#value' => $output,
    '#id' => 'error-message-token-help',
    '#prefix' => '<div>',
    '#suffix' => '</div>',
    '#weight' => 6,
  );

  $info = entity_get_info($instance['entity_type']);
  $token_type = $info['token type'];
  $form['token_help']['tokens'] = array(
    '#theme' => 'token_tree',
    '#token_types' => array($token_type),
    '#dialog' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => (!empty($machine)) ? t('Save rule') : t('Add rule'),
    '#weight' => 25,
  );

  if ($action == 'overwrite') {
    $form['submit']['#value'] = t('Overwrite rule');
  }

  return $form;
}

/**
 * Validation handler to add / edit a rule.
 */
function field_validation_ui_manage_rule_validate($form, &$form_state) {
  $values = $form_state['values'];
  if (FALSE && $values['action'] == 'edit') {
    if (empty($values['machine'])) {
      form_set_error(NULL, t('A problem occurred while editing this rule. Please try again.'));
    }
  }
}

/**
 * Submit handler to add / edit a rule.
 */
function field_validation_ui_manage_rule_submit($form, &$form_state) {
  $values = $form_state['values'];

  // Load current file.
  $field_info = $values['entity_type'] . '.' . $values['bundle'] . '.' . $values['field_name'];
  $config_name = 'field_validation.rule.' . $field_info . '.' . $values['machine'];
  $config = config($config_name);

  // These will be the same every time.
  $config->set('_module', 'field_validation');
  $config->set('entity_type', $values['entity_type']);
  $config->set('bundle', $values['bundle']);
  $config->set('field_name', $values['field_name']);

  // These may change
  $config->set('rulename', $values['rulename']);
  $config->set('machine', $values['machine']);
  $config->set('validator', $values['validator']);
  $config->set('col', $values['col']);
  $config->set('error_message', $values['error_message']);
  $config->set('settings', $values['settings']);

  // Save.
  $config->save();
}

/**
 * Confirmation form to delete a rule.
 */
function field_validation_ui_delete_rule($form, &$form_state, $config_suffix = FALSE) {
  if (isset($config_suffix)) {
    $config_name = 'field_validation.rule.' . $config_suffix;
    $form['config_name'] = array(
      '#type' => 'hidden',
      '#value' => $config_name,
    );

    $rule = config($config_name);
    $human_name = $rule->get('rulename');

    return confirm_form($form,
      t('Are you sure you want to delete the rule %name?', array('%name' => $human_name)),
      isset($_GET['destination']) ? $_GET['destination'] : $_GET['q'],
      t('This action cannot be undone.'),
      t('Delete'),
      t('Cancel')
    );
  }
}

/**
 * Submit handler to delete a rule.
 */
function field_validation_ui_delete_rule_submit($form, &$form_state) {
  $config_name = $form_state['values']['config_name'];
  $config = config($config_name);
  $config->delete();
}

/**
 * Load a validation rule.
 *
 * @param $machine
 *   This validation rule's name value.
 *
 * @return
 *   An array of options for the specified validation rule.
 */
function _field_validation_ui_rule_name_exists($machine) {
  // @todo I don't know how to get these field values from here?!?
  return FALSE;

  $field_info = $values['entity_type'] . '.' . $values['bundle'] . '.' . $values['field_name'];
  $prefix = 'field_validation.rule.' . $field_info;
  $config_names = config_get_names_with_prefix($prefix);
  foreach ($config_names as $config_name) {
    $existing_name = str_replace($prefix, '', $config_name);
    if ($existing_name == $machine) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Theme the 'add rule' list.
 */
function theme_field_validation_ui_manage_add_rule($variables) {
  $instance = $variables['instance'];
  $output = '';

  // Switch from ctools plugins to classes.
  //ctools_include('plugins');
  //$validators = ctools_get_plugins('field_validation', 'validator');

  $validators = field_validation_get_validators();

  if ($validators) {
    $output = '<h3>' . t('Add a validation rule') . '</h3>';
    $output .= '<dl>';
    foreach ($validators as $validator_key => $validator_info) {
      $item = '';
      $path = isset($_GET['q']) ? $_GET['q'] : '';
      $url = $path . '/add/' . $validator_key;
      $item = '<dt>' . l($validator_info['label'], $url, array('query' => backdrop_get_destination())) . '</dt>';
      $item .= '<dd>';
      if (!empty($validator_info['description'])) {
        $item .= $validator_info['description'] . ' ';
      }
      $item .= '</dd>';
      $output .= $item;
    }
    $output .= '</dl>';
  }
  return $output;
}


/**
 * Themable function to list the rules assigned to a field instance.
 */
function theme_field_validation_ui_manage_overview($variables) {
  $rules = $variables['rules'];
  $instance = $variables['instance'];

  $header = array(
    t('Rule name'),
    t('Validator'),
    array(
      'data' => t('Operations'),
      'colspan' => 2,
    ),
  );

  // @todo switch from using ctools plugins to normal classes.
  //ctools_include('plugins');
  //$validators = ctools_get_plugins('field_validation', 'validator');

  $validators = field_validation_get_validators();

  if (!empty($rules)) {
    foreach ($rules as $rule) {
      $machine = $rule['machine'];
      $validator = $rule['validator'];

      $row = array();
      $row[] = array(
        'data' => $rule['rulename'],
      );
      $row[] = array(
        'data' => $validators[$validator]['label'],
      );
      $path = isset($_GET['q']) ? $_GET['q'] : '';
      if (!empty($rule['machine'])) {
        $row[] = array(
          'data' => l(t('Edit'), $path . '/edit/' . $validator . '/' . $machine, array('query' => backdrop_get_destination())),
        );
        $config_suffix = $rule['entity_type'] . '.' . $rule['bundle'] . '.' . $rule['field_name'] . '.' . $machine;
        $row[] = array(
          'data' => l(t('Delete'), $path . '/delete/' . $config_suffix, array('query' => backdrop_get_destination())),
        );
      }
      else {
        $row[] = array(
          'data' => l(t('Overwrite'), $path . '/overwrite/' . $validator . '/' . $machine, array('query' => backdrop_get_destination())),
          'colspan' => 2,
        );
      }
      $rows[] = $row;
    }
  }
  else {
    $rows[][] = array(
      'data' => t('No validation rules available.'),
      'colspan' => 5,
    );
  }

  return theme('table', array('header' => $header, 'rows' => $rows));
}
