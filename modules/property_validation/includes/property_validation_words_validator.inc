<?php
/**
 * @file
 * Property validation words validator.
 */
class property_validation_words_validator extends property_validation_validator {

  /**
   * Validate property.
   */
  public function validate() {
    $settings = $this->rule['settings'];
    if ($this->value != '') {
      $flag = TRUE;
      $count = count(explode(' ', trim(preg_replace('/\s+/', ' ', str_replace('&nbsp;', ' ', (strip_tags(str_replace('<', ' <', $this->value))))))));
      if (isset($settings['min']) && $settings['min'] != '' && $count < $settings['min']) {
        $flag = FALSE;
      }
      if (isset($settings['max']) && $settings['max'] != '' && $count > $settings['max']) {
        $flag = FALSE;
      }

      if (!$flag) {
        $token = array(
          '[min]' => isset($settings['min']) ? $settings['min'] : '',
          '[max]' => isset($settings['max']) ? $settings['max'] : '',
          '[count]' => $count,
        );
        $this->set_error($token);
      }
    }
  }

  /**
   * Provide settings option.
   */
  function settings_form(&$form, &$form_state) {
    $default_settings = $this->get_default_settings($form, $form_state);
    // Print debug($default_settings);
    $form['settings']['min'] = array(
      '#title' => t('Minimum number of words'),
      '#description' => t("Optionally specify the minimum number of words that have to be entered to pass validation. Words are defined as strings of letters separated by spaces."),
      '#type' => 'textfield',
      '#default_value' => isset($default_settings['min']) ? $default_settings['min'] : '',
    );
    $form['settings']['max'] = array(
      '#title' => t('Maximum number of words'),
      '#description' => t("Optionally specify the maximum number of words that have to be entered to pass validation. Words are defined as strings of letters separated by spaces."),
      '#type' => 'textfield',
      '#default_value' => isset($default_settings['max']) ? $default_settings['max'] : '',
    );
    parent::settings_form($form, $form_state);
  }

  /**
   * Provide token help info for error message.
   */
  public function token_help() {
    $token_help = parent::token_help();
    $token_help += array(
      '[min]' => t('Minimum number of words'),
      '[max]' => t('Maximum number of words'),
      '[count]' => t('The real number of words'),
    );
    return $token_help;
  }

}
