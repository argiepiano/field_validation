<?php
/**
 * @file
 * Field validation min length validator.
 *
 */
$plugin = array(
  'label' => t('Min length'),
  'handler' => array(
    'class' => 'field_validation_min_length_validator',
  ),
);

class field_validation_min_length_validator extends field_validation_validator {

  /**
   * Validate field. 
   */
  public function validate() {
    $min_length = $this->rule->settings['data'];
    if ($this->value != '' && (drupal_strlen($this->value) < $min_length)) {
      $error_element = $this->error_element();
      $error_message = $this->error_message();
      form_set_error($error_element,  check_plain($error_message));
    }
  }
  
  /**
   * Provide settings option
   */
  function settings_form(&$form, &$form_state) {
    $form['settings']['data'] = array(
      '#title' => t('Minimum number of characters'),
      '#description' => t("Specify the minimum number of characters that have to be entered to pass validation."),
      '#type' => 'textfield',
      //'#default_value' => $this->options['link_to_user'],
      '#default_value' => '',
    );
  }

}