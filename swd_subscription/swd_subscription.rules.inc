<?php

/**
 * @file
 * Rules integration for Successful Web Developer Subscription functionality
 */

/**
 * Implements hook_rules_event_info().
 */
function swd_subscription_rules_event_info() {
  $items = array(
    'swd_subscription_purchase' => array(
      'label' => t('After purchasing a subscription'),
      'group' => t('SWD Subscription'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User purchasing the subscription'),
        ),
        'product' => array(
          'type' => 'node',
          'bundle' => 'product',
          'label' => t('Product that was purchased'),
        ),
        'billing_period' => array(
          'type' => 'text',
          'label' => t('The billing period of the subscription'),
        ),
        'renewal_date' => array(
          'type' => 'text',
          'label' => t('The renewal date of the subscription'),
        ),
      ),
    ),
    'swd_subscription_renewal_success' => array(
      'label' => t('After a successful subscription automatic renewal'),
      'group' => t('SWD Subscription'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User whose subscription was renewed'),
        ),
        'billing_period' => array(
          'type' => 'text',
          'label' => t('The billing period of the subscription'),
        ),
        'renewal_date' => array(
          'type' => 'text',
          'label' => t('The renewal date of the subscription'),
        ),
      ),
    ),
    'swd_subscription_renewal_failure' => array(
      'label' => t('After an unsuccessful subscription automatic renewal'),
      'group' => t('SWD Subscription'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User whose subscription was not renewed'),
        ),
        'product' => array(
          'type' => 'node',
          'bundle' => 'product',
          'label' => t('Product that failed to renew'),
        ),
        'amount' => array(
          'type' => 'text',
          'label' => t('The amount charged for the subscription'),
        ),
      ),
    ),
    'swd_subscription_missing_renewal_information' => array(
      'label' => t('When missing information to finish processing a renewal'),
      'group' => t('SWD Subscription'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User whose subscription is supposed to be renewed'),
        ),
        'amount' => array(
          'type' => 'text',
          'label' => t('The amount charged for the subscription'),
        ),
        'billing_period' => array(
          'type' => 'text',
          'label' => t('The billing period of the subscription'),
        ),
        'renewal_date' => array(
          'type' => 'text',
          'label' => t('The renewal date of the subscription'),
        ),
      ),
    ),
    'swd_subscription_baid_error' => array(
      'label' => t('When failing to generate a billing agreement'),
      'group' => t('SWD Subscription'),
      'variables' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User who was purchasing a subscription'),
        ),
        'response' => array(
          'type' => 'text',
          'label' => t('The response from the PayPal server'),
        ),
      ),
    ),
  );
  return $items;
}

/**
 * Implements hook_rules_action_info().
 */
function swd_subscription_rules_action_info() {
  $items = array(
    'swd_subscription_grant_role' => array(
      'label' => t('Grant a role for a subscription purchase'),
      'group' => t('SWD Subscription'),
      'parameter' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User who purchased subscription'),
        ),
        'product' => array(
          'type' => 'node',
          'bundles' => array('product'),
          'label' => t('Product that was purchased'),
        ),
      ),
    ),
    'swd_subscription_revoke_role' => array(
      'label' => t('Revoke a role when a subscription expires'),
      'group' => t('SWD Subscription'),
      'parameter' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User whose subscription expired'),
        ),
        'product' => array(
          'type' => 'node',
          'bundles' => array('product'),
          'label' => t('Product that expired'),
        ),
      ),
    ),
    'swd_subscription_clear_billing_agreement_id' => array(
      'label' => t('Clear the billing agreement ID'),
      'group' => t('SWD Subscription'),
      'parameter' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User whose agreement to clear'),
        ),
      ),
    ),
    'swd_subscription_clear_rate_fields' => array(
      'label' => t('Clear the user\'s monthly and annual rates'),
      'group' => t('SWD Subscription'),
      'parameter' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User whose rates to clear'),
        ),
      ),
    ),
    'swd_subscription_set_renewal_date' => array(
      'label' => t('Set the renewal data upon a successful renewal'),
      'group' => t('SWD Subscription'),
      'parameter' => array(
        'account' => array(
          'type' => 'user',
          'label' => t('User whose subscription expired'),
        ),
        'renewal_date' => array(
          'type' => 'text',
          'label' => t('The renewal date of the subscription'),
        ),
      ),
    ),
  );
  return $items;
}

/**
 * Grant a role when a product is purchased
 *
 * @param object $account
 *   The user account to be granted the role
 * @param object $product
 *   The fully loaded node object of the product that was purchased
 */
function swd_subscription_grant_role($account, $product) {
  user_multiple_role_edit(array($account->uid), 'add_role', $product->field_assign_role[LANGUAGE_NONE][0]['rid']);
}

/**
 * Revoke a role when a product expires
 *
 * @param object $account
 *   The user account to be granted the role
 * @param object $product
 *   The fully loaded node object of the expired product
 */
function swd_subscription_revoke_role($account, $product) {
  user_multiple_role_edit(array($account->uid), 'remove_role', $product->field_assign_role[LANGUAGE_NONE][0]['rid']);
}

/**
 * Clear the Billing Agreement ID when a renewal fails
 *
 * @param object $account
 *   The user account from which to remove the ID
 */
function swd_subscription_clear_billing_agreement_id($account) {
  $account_wrapper = entity_metadata_wrapper('user', $account);
  $account_wrapper->field_billing_agreement_id->set();
  $account_wrapper->save();
}

/**
 * Clear the user rate fields
 *
 * @param object $account
 *   The user account from which to clear the rates
 */
function swd_subscription_clear_rate_fields($account) {
  $account_wrapper = entity_metadata_wrapper('user', $account);
  $account_wrapper->field_monthly_cost->set();
  $account_wrapper->field_annual_cost->set();
  $account_wrapper->save();
}

/**
 * Set the renewal date on a successful renewal
 *
 * @param object $account
 *   The user account from which to remove the ID
 * @param string $renewal_date
 *   The renewal date in YYYYMMDD format
 */
function swd_subscription_set_renewal_date($account, $renewal_date) {
  $account_wrapper = entity_metadata_wrapper('user', $account);
  $account_wrapper->field_renewal_date->set($renewal_date);
  $account_wrapper->save();
}
