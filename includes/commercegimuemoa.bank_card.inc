<?php

/**
 * @file
 * Credit-card helper functions for Drupal commerce.
 */
function commercegimuemoa_bank_card_form($fields = array(), $default = array()) {
  // Merge default values into the default array.
  $default += array(
    'type' => '',
    'number' => '',
    'start_month' => '',
    'start_year' => date('Y') - 5,
    'exp_month' => date('m'),
    'exp_year' => date('Y'),
    'issue' => '',
    'code' => '',
    'bank' => '',
  );

  $current_year_2 = date('y');
  $current_year_4 = date('Y');

  $form['bank_card'] = array(
    '#tree' => TRUE,
    '#attached' => array(
      'css' => array(drupal_get_path('module', 'commercegimuemoa') . '/theme/commercegimuemoa.theme.css'),
    ),
  );

  // Add a card type selector if specified.
  if (isset($fields['type'])) {
    $form['bank_card']['type'] = array(
      '#type' => 'select',
      '#title' => t('Card type'),
      '#options' => array_intersect_key(commercegimuemoa_bank_card_types(), drupal_map_assoc((array) $fields['type'])),
      '#default_value' => $default['type'],
    );
    $form['bank_card']['valid_types'] = array(
      '#type' => 'value',
      '#value' => $fields['type'],
    );
  }
  else {
    $form['bank_card']['valid_types'] = array(
      '#type' => 'value',
      '#value' => array(),
    );
  }



  // Always add a field for the credit card number.
  $form['bank_card']['number'] = array(
    '#type' => 'textfield',
    '#title' => t('Card number'),
    '#default_value' => $default['number'],
    '#attributes' => array('autocomplete' => 'off'),
    '#required' => TRUE,
    '#maxlength' => 16,
    '#size' => 16,
  );

  // Add fields for the credit card start date if specified.
  if (isset($fields['start_date'])) {
    $form['bank_card']['start_month'] = array(
      '#type' => 'select',
      '#title' => t('Start date'),
      '#options' => drupal_map_assoc(array_keys(commerce_months())),
      '#default_value' => strlen($default['start_month']) == 1 ? '0' . $default['start_month'] : $default['start_month'],
      '#required' => TRUE,
      '#prefix' => '<div class="commerce-credit-card-start">',
      '#suffix' => '<span class="commerce-month-year-divider">/</span>',
    );

    // Build a year select list that uses a 4 digit key with a 2 digit value.
    $options = array();

    for ($i = -10; $i < 1; $i++) {
      $options[$current_year_4 + $i] = str_pad($current_year_2 + $i, 2, '0', STR_PAD_LEFT);
    }

    $form['bank_card']['start_year'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $default['start_year'],
      '#suffix' => '</div>',
    );
  }

  // Always add fields for the credit card expiration date.
  $form['bank_card']['exp_month'] = array(
    '#type' => 'select',
    '#title' => t('Expiration'),
    '#options' => drupal_map_assoc(array_keys(commerce_months())),
    '#default_value' => strlen($default['exp_month']) == 1 ? '0' . $default['exp_month'] : $default['exp_month'],
    '#required' => TRUE,
    '#prefix' => '<div class="commerce-credit-card-expiration">',
    '#suffix' => '<span class="commerce-month-year-divider">/</span>',
  );

  // Build a year select list that uses a 4 digit key with a 2 digit value.
  $options = array();

  for ($i = 0; $i < 20; $i++) {
    $options[$current_year_4 + $i] = str_pad($current_year_2 + $i, 2, '0', STR_PAD_LEFT);
  }

  $form['bank_card']['exp_year'] = array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $default['exp_year'],
    '#suffix' => '</div>',
  );

  // Add a field for the card issue number if specified.
  if (isset($fields['issue'])) {
    $form['bank_card']['issue'] = array(
      '#type' => 'textfield',
      '#title' => t('Issue number', array(), array('context' => 'credit card issue number for card types that require it')),
      '#default_value' => $default['issue'],
      '#attributes' => array('autocomplete' => 'off'),
      '#required' => empty($fields['issue']) ? FALSE : TRUE,
      '#maxlength' => 2,
      '#size' => 2,
    );
  }

  // Add a field for the security code if specified.
  if (isset($fields['code'])) {
    $form['bank_card']['code'] = array(
      '#type' => 'textfield',
      '#title' => !empty($fields['code']) ? $fields['code'] : t('Security code'),
      '#default_value' => $default['code'],
      '#attributes' => array('autocomplete' => 'off'),
      '#required' => TRUE,
      '#maxlength' => 4,
      '#size' => 4,
    );
  }

  // Add a field for the issuing bank if specified.
  if (isset($fields['bank'])) {
    $form['bank_card']['bank'] = array(
      '#type' => 'textfield',
      '#title' => t('Issuing bank'),
      '#default_value' => $default['bank'],
      '#attributes' => array('autocomplete' => 'off'),
      '#required' => TRUE,
      '#maxlength' => 64,
      '#size' => 32,
    );
  }

  return $form;
}

/**
 * Validates a set of credit card details entered via the credit card form.
 *
 * @param $details
 *   An array of credit card details as retrieved from the credit card array in
 *   the form values of a form containing the credit card form.
 * @param $settings
 *   Settings used for calling validation functions and setting form errors:
 *   - form_parents: an array of parent elements identifying where the credit
 *     card form was situated in the form array
 *
 * @return
 *   TRUE or FALSE indicating the validity of all the data.
 *
 * @see commerce_payment_credit_card_form()
 */
function commercegimuemoa_bank_card_validate($details, $settings) {
  $prefix = implode('][', $settings['form_parents']) . '][';
  $valid = TRUE;

  // Validate the credit card type.
  if (!empty($details['valid_types'])) {
    $type = commercegimuemoa_validate_bank_card_type($details['number'], $details['valid_types']);

    if ($type === FALSE) {
      form_set_error($prefix . 'type', t('Vous avez entré un numéro de carte qui nest pas du reseau GIM-UEMOA.'));
      $valid = FALSE;
    }
    elseif ($type != $details['type']) {
      form_set_error($prefix . 'number', t('Vous avez entré un numéro de carte  qui ne correspond pas au type sélectionné.'));
      $valid = FALSE;
    }
  }

  // Validite du  numero de la carte
  if (!commercegimuemoa_validate_bank_card_number($details['number'])) {
    form_set_error($prefix . 'number', t('Vous avez entré un numéro de carte de crédit invalide.'));
    $valid = FALSE;
  }

  // Validite de la date dexpiration.
  if (($invalid = commercegimuemoa_validate_bank_card_exp_date($details['exp_month'], $details['exp_year'])) !== TRUE) {
    form_set_error($prefix . 'exp_' . $invalid, t('Vous avez entré une carte expirée.'));
    $valid = FALSE;
  }

  // Validite du code CVV2.
  if (isset($details['code']) && !commercegimuemoa_validate_bank_card_security_code($details['number'], $details['code'])) {
    form_set_error($prefix . 'code', t('Vous avez entré un code de sécurité de carte invalide.'));
    $valid = FALSE;
  }

//  // Validate the start date if present.
//  if (isset($details['start_month']) && ($invalid = commercegimuemoa_validate_bank_card_start_date($details['start_month'], $details['start_year'])) !== TRUE) {
//    form_set_error($prefix . 'start_' . $invalid, t('Your have entered an invalid start date.'));
//    $valid = FALSE;
//  }
//  // Validate the issue number if present.
//  if (isset($details['issue']) && !commercegimuemoa_validate_bank_card_issue($details['issue'])) {
//    form_set_error($prefix . 'issue', t('You have entered an invalid issue number.'));
//    $valid = FALSE;
//  }

  return $valid;
}

/**
 * Validates a credit card number using an array of approved card types.
 *
 * @param $number
 *   The credit card number to validate.
 * @param $card_types
 *   An array of credit card types containing any of the keys from the array
 *   returned by commerce_payment_credit_card_types(). Only numbers determined
 *   to be of the types specified will pass validation. This determination is
 *   based on the length of the number and the valid number ranges for the
 *   various types of known credit card types.
 *
 * @return
 *   FALSE if a number is not valid based on approved credit card types or the
 *   credit card type if it is valid and coud be determined.
 *
 * @see http://en.wikipedia.org/wiki/Bank_card_number#Issuer_Identification_Number_.28IIN.29
 * @see commerce_payment_credit_card_types()
 */
function commercegimuemoa_validate_bank_card_type($number, $card_types) {
  $strlen = strlen($number);

  // Provide a check on the first digit (and card length if applicable).
  switch (substr($number, 0, 1)) {


//    case '2':
//      $initial = (int) substr($number, 0, 4);
//      $return = FALSE;
//
//      if ($strlen == 16) {
//        // Ecobank
//        if (in_array('ecobank', $card_types)) {
//          $return = 'ecobank';
//        }
//
//        // Visa Electron begins with 4026, 417500, 4256, 4508, 4844, 4913, or
//        // 4917 and is 16 numbers.
//        if (in_array($initial, array(2026, 2256, 2508, 2844, 2913, 2917))) {
//          $return = in_array('bislamique', $card_types) ? 'bislamique' : FALSE;
//        }
//         if ($initial >= 2351 && $initial <= 2355 && in_array('batlantique', $card_types)) {
//          return 'batlantique';
//        }
//
//      }
//
//      
//
//      return $return;



    case '5':
      // Discover begins with 6011, 622126-622925, 644-649, or 65 and is 16
      // numbers.
      if ($strlen == 16 && in_array('ecobank', $card_types)) {
        if (substr($number, 0, 4) == '5326') {
          return 'ecobank';
        }
      }

      if ($strlen == 16 && in_array('bislamique', $card_types)) {
        if (substr($number, 0, 4) == '5327') {
          return 'bislamique';
        }
      }
      if ($strlen == 16 && in_array('batlantique', $card_types)) {
        if (substr($number, 0, 4) == '5328') {
          return 'batlantique';
        }
      }
      if ($strlen == 16 && in_array('gim', $card_types)) {
        if (substr($number, 0, 4) == '5329') {
          return 'gim';
        }
      }

//      // Switch begins with 633110, 6333, or 6759 and is 16, 18, or 19 numbers.
//      if (in_array('switch', $card_types) && in_array($strlen, array(16, 18, 19)) &&
//        (in_array($initial, array(6333, 6759)) || substr($number, 0, 6) == 633110)) {
//        return 'switch';
//      }

      return FALSE;
  }

  return FALSE;
}

/**
 * Validates a credit card number using the Luhn algorithm.
 *
 * @param $number
 *   The credit card number to validate.
 *
 * @return
 *   TRUE or FALSE indicating the number's validity.
 *
 * @see http://www.merriampark.com/anatomycc.htm
 */
function commercegimuemoa_validate_bank_card_number($number) {
  // Ensure every character in the number is numeric.
  if (!ctype_digit($number)) {
    return FALSE;
  }

  // Validate the number using the Luhn algorithm.
  $total = 0;

  for ($i = 0; $i < strlen($number); $i++) {
    $digit = substr($number, $i, 1);
    if ((strlen($number) - $i - 1) % 2) {
      $digit *= 2;
      if ($digit > 9) {
        $digit -= 9;
      }
    }
    $total += $digit;
  }

  if ($total % 10 != 0) {
    return FALSE;
  }

  return TRUE;
}

/**
 * Validates a credit card start date.
 *
 * @param $month
 *   The 1 or 2-digit numeric representation of the month, i.e. 1, 6, 12.
 * @param $year
 *   The 4-digit numeric representation of the year, i.e. 2010.
 *
 * @return
 *   TRUE for cards whose start date is blank (both month and year) or in the
 *   past, 'year' or 'month' for expired cards indicating which value should
 *   receive the error.
 */
function commercegimuemoa_validate_bank_card_start_date($month, $year) {
  if (empty($month) && empty($year)) {
    return TRUE;
  }

  if (empty($month) || empty($year)) {
    return empty($month) ? 'month' : 'year';
  }

  if ($month < 1 || $month > 12) {
    return 'month';
  }

  if ($year > date('Y')) {
    return 'year';
  }
  elseif ($year == date('Y')) {
    if ($month > date('n')) {
      return 'month';
    }
  }

  return TRUE;
}

/**
 * Validates a credit card expiration date.
 *
 * @param $month
 *   The 1 or 2-digit numeric representation of the month, i.e. 1, 6, 12.
 * @param $year
 *   The 4-digit numeric representation of the year, i.e. 2010.
 *
 * @return
 *   TRUE for non-expired cards, 'year' or 'month' for expired cards indicating
 *   which value should receive the error.
 */
function commercegimuemoa_validate_bank_card_exp_date($month, $year) {
  if ($month < 1 || $month > 12) {
    return 'month';
  }

  if ($year < date('Y')) {
    return 'year';
  }
  elseif ($year == date('Y')) {
    if ($month < date('n')) {
      return 'month';
    }
  }

  return TRUE;
}

/**
 * Validates that an issue number is numeric if present.
 */
//function commercegimuemoa_validate_bank_card_issue($issue) {
//  if (empty($issue) || (is_numeric($issue) && $issue > 0)) {
//    return TRUE;
//  }
//
//  return FALSE;
//}

/**
 * Validates a card security code based on the type of the credit card.
 *
 * @param $number
 *   The number of the credit card to validate the security code against.
 * @param $code
 *   The card security code to validate with the given number.
 *
 * @return
 *   TRUE or FALSE indicating the security code's validity.
 */
function commercegimuemoa_validate_bank_card_security_code($number, $code) {
  // Ensure the code is numeric.
  if (!ctype_digit($code)) {
    return FALSE;
  }

  // Check the length based on the type of the credit card.
  switch (substr($number, 0, 1)) {
    case '2':
      if (strlen($number) == 16) {
        return strlen($code) == 3;
      }
      else {
        return strlen($code) == 3;
      }

    case '4':
    case '5':
    case '6':
      return strlen($code) == 3;
  }
}

/**
 * Returns an associative array of credit card types.
 */
function commercegimuemoa_bank_card_types() {
  return array(
    'ecobank' => t('ecobank'),
    'bislamique' => t('bislamique'),
    'batlantique' => t('batlantique'),
    'gim' => t('gim'),
    'switch' => t('Switch'),
  );
}
