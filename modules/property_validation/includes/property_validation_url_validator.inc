<?php
/**
 * @file
 * Property validation url validator.
 */
class property_validation_url_validator extends property_validation_validator {

  /**
   * Validate property.
   */
  public function validate() {
    $settings = $this->rule['settings'];

    if ($this->value != '') {
      $flag = FALSE;
      if (empty($settings['external']) && empty($settings['internal'])) {
        $flag = TRUE;
      }

      if (!empty($settings['external'])) {
        $flag = valid_url($this->value, TRUE);
      }

      if (!$flag && !empty($settings['internal'])) {
        $normal_path = backdrop_get_normal_path($this->value);
        if (!url_is_external($normal_path)) {
          $parsed_link = parse_url($normal_path);
          if ($normal_path != $parsed_link['path']) {
            $normal_path = $parsed_link['path'];
          }
          $flag = backdrop_valid_path($normal_path);
        }
      }

      if (!$flag) {
        $this->set_error();
      }
    }
  }

  /**
   * Provide settings option.
   */
  function settings_form(&$form, &$form_state) {
    $default_settings = $this->get_default_settings($form, $form_state);
    // Print debug($default_settings);
    $form['settings']['external'] = array(
      '#title' => t('External URL'),
      '#description' => t("Limit allowed input to absolute/external url."),
      '#type' => 'checkbox',
      '#default_value' => isset($default_settings['external']) ? $default_settings['external'] : FALSE,
    );
    $form['settings']['internal'] = array(
      '#title' => t('Internal path'),
      '#description' => t("Limit allowed input to internal Backdrop path."),
      '#type' => 'checkbox',
      '#default_value' => isset($default_settings['internal']) ? $default_settings['internal'] : FALSE,
    );
    $form['settings']['help'] = array(
      '#markup' => t("If both of External URL and Internal path are checked, it means that both of them are allowed."),
    );
    parent::settings_form($form, $form_state);
  }

}
