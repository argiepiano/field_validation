<?php
/**
 * @file
 * Field validation php validator.
 */
class field_validation_php_validator extends field_validation_validator {

  /**
   * Validate field.
   */
  public function validate() {
    return eval($this->rule['settings']['data']);
  }

  /**
   * Provide settings option.
   */
  function settings_form(&$form, &$form_state) {
    $default_settings = $this->get_default_settings($form, $form_state);
    // Print debug($default_settings);
    $form['settings']['data'] = array(
      '#title' => t('PHP code'),
      '#description' => t("Be careful, it is powerful and also dangerous. Do not use &lt;?php ?&gt; at here, for document: http://drupal.org/node/1537028."),
      '#type' => 'textarea',
      '#default_value' => isset($default_settings['data']) ? $default_settings['data'] : '',
    );
    parent::settings_form($form, $form_state);
  }

}
