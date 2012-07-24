
/**
 * @file
 * Attaches behaviors for the Path module.
 */

(function ($) {

Drupal.behaviors.pbjFieldsetSummaries = {
  attach: function (context) {
    $('fieldset.pbj-form', context).drupalSetSummary(function (context) {
      var pbj = $('.form-item-pbj-field option:selected').text();

      return pbj ?
        Drupal.t('Field: @alias', { '@alias': pbj }) :
        Drupal.t('No Jelly with your Butter');
    });
  }
};

})(jQuery);
