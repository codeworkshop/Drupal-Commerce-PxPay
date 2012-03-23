<?php

/**
 * @file
 * PxPay administration menu items.
 * @author Robbie Mackay <rm@robbiemackay.com>
 * @copyright Robbie MacKay 2009
 */

// Handles a completed PxPay sale.
function commerce_dps_pxpay_complete($order, $payment_method) {
 
  include('PxPay_Curl.inc');

  $pxpay = new PxPay_Curl(
    variable_get('commerce_dps_pxpay_server', 'https://sec.paymentexpress.com/pxpay/pxaccess.aspx'),
    variable_get('commerce_dps_pxpay_userid', ''),
    variable_get('commerce_dps_pxpay_key', '')
  );

  $enc_hex = $_GET["result"];
  // getResponse method in PxPay object returns PxPayResponse object
  // which encapsulates all the response data
  $rsp = $pxpay->getResponse($enc_hex);

  
  // the following are the fields available in the PxPayResponse object
  $order->data['payment_details']['pxpay_Success'] = $rsp->getSuccess();   # =1 when request succeeds
  $payment_amount = $order->data['payment_details']['pxpay_AmountSettlement']  = $rsp->getAmountSettlement();
  $order->data['payment_details']['pxpay_AuthCode'] = $rsp->getAuthCode();  # from bank
  $order->data['payment_details']['pxpay_CardName'] = $rsp->getCardName();  # eg "Visa"
  $order->data['payment_details']['pxpay_CardNumber'] = $rsp->getCardNumber(); # Truncated card number
  $order->data['payment_details']['pxpay_CardHolderName'] = $rsp->getCardHolderName();

  $order->data['payment_details']['pxpay_DateExpiry'] = $rsp->getDateExpiry();
  $txn_id = $rsp->getDpsTxnRef();
  $order->data['payment_details']['pxpay_TxnType'] = $rsp->getTxnType();
  $order->data['payment_details']['pxpay_ClientInfo'] = $rsp->getClientInfo(); # The IP address of the user who submitted the transaction
  $order->data['payment_details']['pxpay_TxnId'] = $rsp->getTxnId();
  $order->data['payment_details']['pxpay_EmailAddress'] = $rsp->getEmailAddress();
  $order->data['payment_details']['pxpay_MerchantReference'] = $rsp->getMerchantReference();
  $order->data['payment_details']['pxpay_ResponseText'] = $rsp->getResponseText();

  if ($rsp->getSuccess() == "1") {
    // Create a new payment transaction for the order.
    $transaction = commerce_payment_transaction_new('commerce_dps_pxpay', $order->order_number);
    $transaction->instance_id = $payment_method['instance_id'];
    $transaction->remote_id = $txn_id;
    $transaction->amount = commerce_currency_decimal_to_amount($payment_amount);
    $transaction->currency_code = variable_get('commerce_dps_pxpay_currency', 'NZD');
    $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
    $transaction->message = 'MerchantReference=@merchant_reference. ResponseText=@response_text';
    $transaction->message_variables =
      array(
        '@merchant_reference' => $order->data['payment_details']['pxpay_MerchantReference'],
        '@response_text' => $order->data['payment_details']['pxpay_ResponseText'],
    );

    // Save the transaction information.
    commerce_payment_transaction_save($transaction);
  
    commerce_payment_redirect_pane_next_page($order);
    watchdog('commerce_dps_pxpay', 'DPS processed for Order @order_number with ID @txn_id.', array('@txn_id' => $txn_id], '@order_number' => $order->order_number), WATCHDOG_INFO);
  }
}

// Handles a canceled Website Payments Standard sale.
function commerce_dps_pxpay_cancel($order) {
  commerce_payment_redirect_pane_previous_page($order);
}

