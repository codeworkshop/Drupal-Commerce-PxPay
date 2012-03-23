<?php

/**
 * 1. Generate Request
 * If request is valid, return URL to redirect to.
 */
function commerce_dps_pxpay_generate_request($transaction) {
  // ensure amount is dd.cc
  $transaction['amount'] = number_format($transaction['amount'], 2, '.', '');

  $xml = '<GenerateRequest>';
  $xml .= '  <PxPayUserId>' . $transaction['user_id'] . '</PxPayUserId>';
  $xml .= '  <PxPayKey>' . $transaction['key'] . '</PxPayKey>';
  $xml .= '  <TxnType>' . $transaction['type'] . '</TxnType>';
  $xml .= '  <CurrencyInput>' . $transaction['currency'] . '</CurrencyInput>';
  $xml .= '  <AmountInput>' . $transaction['amount'] . '</AmountInput>';
  $xml .= '  <MerchantReference>' . $transaction['reference'] . '</MerchantReference>';
  $xml .= '  <EmailAddress>' . $transaction['email'] . '</EmailAddress>';
  $xml .= '  <TxnId>' . $transaction['txn_id'] . '</TxnId>';
  $xml .= '  <UrlSuccess>' . $transaction['url_success'] . '</UrlSuccess>';
  $xml .= '  <UrlFail>' . $transaction['url_failure'] . '</UrlFail>';
  $xml .= '</GenerateRequest>';

  if ($transaction['log']) {
    watchdog('commerce_dps_pxpay', 'GenerateRequest: @reference, @xml', array('@reference' => $transaction['reference'], '@xml' => $xml), WATCHDOG_DEBUG);
  }

  $url = $transaction['server'];
  $response = drupal_http_request($url, array('method' => 'POST', 'data' => $xml));

  if ($transaction['log']) {
    watchdog('commerce_dps_pxpay', 'GenerateRequest response: @reference, HTTP @code, @xml', array('@reference' => $transaction['reference'], '@code' => $response->code, '@xml' => $response->data), WATCHDOG_DEBUG);
  }

  // If response from PX Pay is ok, extract and return Hosted Payment Page URI to redirect user to.
  if ($response->code == 200) {
    if ($xml = simplexml_load_string($response->data)) {
      foreach ($xml->attributes() as $attribute => $value) {
        if ($attribute == 'valid' && $value == '1') {
          return $xml->URI;
        }
        else {
          watchdog('commerce_dps_pxpay', 'GenerateRequest invalid: @reference, XML: @xml', array('@reference' => $transaction['reference'], '@xml' => $response->data), WATCHDOG_ERROR);
        }
      }
    }
  }
  watchdog('commerce_dps_pxpay', 'GenerateRequest: @reference, HTTP @code, XML: @xml', array('@reference' => $reference, '@code' => $response->code, '@xml' => $response->data), WATCHDOG_ERROR);
  return FALSE;
}

/**
 * Generate XML for PX Pay Process Response
 *
 * @param string $userid User id assigned by DPS
 * @param string $key Merchant account key assigned by DPS
 * @param string $response Encryped transaction data sent from DPS.
 *
 * @return array $data Transaction data.
 */
function commerce_dps_pxpay_process_response($transaction) {
  // in testing, I'm sure I saw this work without setting
  // PxPayUserId ... weird.
  $xml = '<ProcessResponse>';
  $xml .= '<PxPayUserId>' . $transaction['user_id'] . '</PxPayUserId>';
  $xml .= '<PxPayKey>' . $transaction['key'] . '</PxPayKey>';
  $xml .= '<Response>' . $transaction['result'] . '</Response>';
  $xml .= '</ProcessResponse>';

  $url = variable_get('commerce_dps_pxpay_server', 'https://sec.paymentexpress.com/pxpay/pxaccess.aspx');
  //flog_it(__FUNCTION__ . ': url=' . $url . ', xml=' . $xml);
  $response = drupal_http_request($url, array('method' => 'POST', 'data' => $xml));

  if ($transaction['log']) {
    watchdog('commerce_dps_pxpay', 'ProcessResponse response: HTTP @code, @xml', array('@code' => $response->code, '@xml' => $response->data), WATCHDOG_DEBUG);
  }

  // 6. Response XML Document. If response from PX Pay is good, extract and return transaction data.
  if ($response->code == 200) {
    $xml = simplexml_load_string($response->data);
    // Build data array.
    foreach ($xml->attributes() as $attribute => $value) {
      if ($attribute == 'valid' && $value == '1') {
        $data = array();
        // Since response XML is only one level deep, simply iterate over XML elements and gather values.
        foreach ($xml->children() as $child) {
          $data[$child->getName()] = (string)$child;
        }
        return $data;
      }
      else {
        // @todo: send to watchdog.
      }
    }
  }
  return FALSE;
}

/**
 * Returns an array of possible currency codes.
 */
function commerce_dps_pxpay_currencies() {
  return array(
    'CAD' => 'Canadian Dollar',
    'CHF' => 'Swiss Franc',
    'EUR' => 'Euro',
    'FRF' => 'French Franc',
    'GBP' => 'United Kingdom Pound',
    'HKD' => 'Hong Kong Dollar',
    'JPY' => 'Japanese Yen',
    'NZD' => 'New Zealand Dollar',
    'SGD' => 'Singapore Dollar',
    'USD' => 'United States Dollar',
    'ZAR' => 'Rand',
    'AUD' => 'Australian Dollar',
    'WST' => 'Samoan Tala',
    'VUV' => 'Vanuatu Vatu',
    'TOP' => 'Tongan Pa\'anga',
    'SBD' => 'Solomon Islands Dollar',
    'PNG' => 'Papua New Guinea Kina',
    'MYR' => 'Malaysian Ringgit',
    'KWD' => 'Kuwaiti Dinar',
    'FJD' => 'Fiji Dollar',
  );
}

