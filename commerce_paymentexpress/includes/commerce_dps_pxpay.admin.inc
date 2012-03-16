<?php

/**
 * @file
 * Administration pages for DPS PaPay payment gateway module.
 */

/**
 * Administration settings form.
 */

function commerce_dps_pxpay_admin_settings_form($form, &$form_state) {
  $form['commerce_dps_pxpay_userid'] = array(
    '#type' => 'textfield',
    '#title' => t('PxPay User Id'),
    '#description' => t('PxPay User Id that was issued by Payment Express.'),
    '#default_value' => variable_get('commerce_dps_pxpay_userid', 'Sample User'),
    '#required' => TRUE,
  );
  $form['commerce_dps_pxpay_key'] = array(
    '#type' => 'textfield',
    '#title' => t('PxPay key'),
    '#description' => t('PxPay Key that was issued by Payment Express.'),
    '#default_value' => variable_get('commerce_dps_pxpay_key', 'Sample Key'),
    '#required' => TRUE,
  );
  $form['commerce_dps_pxpay_server'] = array(
    '#type' => 'select',
    '#title' => t('PxPay server'),
    '#description' => t('Sign up for and use a Sandbox account for testing.'),
    '#options' => array(
      'https://sec.paymentexpress.com/pxpay/pxaccess.aspx' => ('Live'),
    ),
    '#default_value' => variable_get('commerce_dps_pxpay_server', 'https://sec.paymentexpress.com/pxpay/pxaccess.aspx'),
    '#required' => TRUE,
  );
  $form['commerce_dps_pxpay_currency'] = array(
    '#type' => 'select',
    '#title' => t('Currency code'),
    '#description' => t('Transactions can only be processed in one of the listed currencies.'),
    '#options' => _commerce_dps_pxpay_currency_array(),
    '#default_value' => variable_get('commerce_dps_pxpay_currency', 'NZD'),
  );
  $form['commerce_dps_pxpay_cancel_return_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Cancel return URL'),
    '#description' => t('Specify the path customers who cancel their DPS PxPay payment will be directed to when they return to your site.'),
    '#default_value' => variable_get('commerce_dps_pxpay_cancel_return_url', 'cart'),
    '#size' => 32,
    '#field_prefix' => url(NULL, array('absolute' => TRUE)) . (variable_get('clean_url', 0) ? '' : '?q='),
  );
  $form['commerce_dps_pxpay_submit_button'] = array(
    '#type' => 'textfield',
    '#title' => t('Submit button text'),
    '#description' => t('Submit button text.'),
    '#default_value' => variable_get('commerce_dps_pxpay_submit_button', 'Submit Order'),
    '#required' => TRUE,
  );
  $form['commerce_dps_pxpay_refprefix'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant Reference Prefix'),
    '#description' => t('Added before order number sent to DPS, ie Website Order #1234'),
    '#default_value' => variable_get('commerce_dps_pxpay_refprefix', 'Website Order'),
    '#required' => TRUE,
  );
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  
  return $form;
}

/**
 * Validation for administration settings.
 */
function commerce_dps_pxpay_admin_settings_form_validate($form, &$form_state) {
  if ((integer) trim($form_state['values']['commerce_dps_pxpay_key']) !== 64) {
    form_set_error('commerce_dps_pxpay_key', t('DPS PxPay account key must be 64 characters long'));
  }
}

/**
 * Submit handler for administration settings.
 */
function commerce_dps_pxpay_admin_settings_form_submit($form, &$form_state) {  
  // Save our new settings.
  variable_set('commerce_dps_pxpay_userid', $form_state['values']['commerce_dps_pxpay_userid']);
  variable_set('commerce_dps_pxpay_key', $form_state['values']['commerce_dps_pxpay_key']);
  variable_set('commerce_dps_pxpay_server', $form_state['values']['commerce_dps_pxpay_server']);
  variable_set('commerce_dps_pxpay_currency', $form_state['values']['commerce_dps_pxpay_currency']);
  variable_set('commerce_dps_pxpay_cancel_return_url', $form_state['values']['commerce_dps_pxpay_cancel_return_url']);
  variable_set('commerce_dps_pxpay_submit_button', $form_state['values']['commerce_dps_pxpay_submit_button']);
  variable_set('commerce_dps_pxpay_refprefix', $form_state['values']['commerce_dps_pxpay_refprefix']);
  
  drupal_set_message(t('Settings have been saved.'));
}