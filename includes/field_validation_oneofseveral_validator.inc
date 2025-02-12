<?php
/**
 * @file
 * Field validation one of several validator.
 */
class field_validation_oneofseveral_validator extends field_validation_validator {

  /**
   * Validate field.
   */
  public function validate() {
    $flag = FALSE;

    $group_name = $this->rule['settings']['data'];
    $field_values = $this->get_field_column_value($this->items, $this->rule->col);
    $field_values = array_flip($field_values);
    if (count($field_values) > 0) {
      $flag = TRUE;
    }
    if (!$flag) {
      ctools_include('export');
      $other_group_rules = ctools_export_load_object('field_validation_rule', 'conditions', array('entity_type' => $this->rule->entity_type, 'bundle' => $this->rule->bundle, 'validator' => $this->rule->validator));
      foreach ($other_group_rules as $other_group_rule) {
        // Skip when do not have the same group name, rule is disabled, equal to current rule.
        if ($other_group_rule->settings['data'] != $group_name || !empty($other_group_rule->disabled) || $other_group_rule->name == $this->rule->name) {
          continue;
        }
        $other_items = isset($this->entity->{$other_group_rule->field_name}[$this->langcode]) ? $this->entity->{$other_group_rule->field_name}[$this->langcode] : array();
        $other_field_values = $this->get_field_column_value($other_items, $other_group_rule->col);
        $other_field_values = array_flip($other_field_values);
        if (count($other_field_values) > 0) {
          $flag = TRUE;
          break;
        }
      }
    }

    if (!$flag) {
      $this->set_error();
    }
    $break = TRUE;
    return $break;
  }

  /**
   * Provide settings option.
   */
  function settings_form(&$form, &$form_state) {
    $default_settings = $this->get_default_settings($form, $form_state);
    // Print debug($default_settings);
    $form['settings']['data'] = array(
      '#title' => t('Group name'),
      '#description' => t("Specify the group name for those fields, it should be the same across those fields. Validation rules with the same group name work together."),
      '#type' => 'textfield',
      '#default_value' => isset($default_settings['data']) ? $default_settings['data'] : '',
    );
    parent::settings_form($form, $form_state);
  }

  /**
   * Helper function to get field values.
   */
  function get_field_column_value($items, $column = 'value') {
    $field_values = array();
    foreach ($items as $delta => $item) {
      if (isset($item[$column]) && $item[$column] != '') {
        $field_values[] = $item[$column];
      }
    }
    return $field_values;
  }

}
