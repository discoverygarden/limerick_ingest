<?php
/**
 * @file
 * Drush integration.
 */

/**
 * Implements hook_drush_command().
 */
function limerick_ingest_drush_command() {
  return array(
    'limerick_ingest' => array(
      'bootstrap' => DRUSH_BOOTSTRAP_MAX,
      'description' => 'Ingest object(s) into Islandora.',
      'drupal_dependencies' => array('islandora'),
      'examples' => array(
        'compound_images' => 'drush -u 1 -v limerick_ingest --parent=islandora:root --namespace=namespace --type=compound_images --data_target=/some_dir --metadata_target=/some_other_dir',
        'book' => 'drush -u 1 -v limerick_ingest --parent=islandora:root --namespace=namespace --type=book --data_target=/some_dir --metadata_target=/some_other_dir',
        'images' => 'drush -u 1 -v limerick_ingest --parent=islandora:root --namespace=namespace --type=images --data_target=/some_dir --metadata_target=/some_other_dir',
        'recursive_objects' => 'drush -u 1 -v limerick_ingest --parent=islandora:root --namespace=namespace --type=recursive_objects --data_target=/some_dir --metadata_target=/some_other_dir',
        'issues' => 'drush -u 1 -v limerick_ingest --parent=islandora:root --namespace=namespace --type=issues --data_target=/some_dir --metadata_target=/some_other_dir',
        'postcards' => 'drush -u 1 -v limerick_ingest --parent=islandora:root --namespace=namespace --type=postcards --data_target=/some_dir --metadata_target=/some_other_dir',
      ),
      'options' => array(
        'type' => array(
          'description' => dt('The type of ingest (compound_images, book, images, recursive_objects, issues, postcards).'),
        ),
        'data_target' => array(
          'description' => dt('Directory to take data from.'),
        ),
        'metadata_target' => array(
          'description' => dt('Directory to take metadata from.'),
        ),
        'namespace' => array(
          'description' => dt('Namespace to ingest into.'),
        ),
        'parent' => array(
          'description' => dt('Parent to ingest into.'),
        ),
      ),
    ),
  );
}

/**
 * Drush ingest callback.
 */
function drush_limerick_ingest() {
  module_load_include('inc', 'limerick_ingest', 'includes/ingest');
  // XXX: Due to how Drush bootstrapping works, the connection may be created
  // without credentials (when your site's front page is
  // 'islandora/object/some:object', for example). Resetting to ensure a new
  // connection gets created should fix it.
  drupal_static_reset('islandora_get_tuque_connection');
  $connection = islandora_get_tuque_connection();
  $parameters = array(
    'target' => drush_get_option('data_target'),
    'metadata_target' => drush_get_option('metadata_target'),
    'namespace' => drush_get_option('namespace'),
    'parent' => drush_get_option('parent'),
    'parent_relationship_pred' => 'isMemberOfCollection',
    'parent_relationship_uri' => FEDORA_RELS_EXT_URI,
  );
  limerick_ingest_ingest(drush_get_option('type'), $parameters, $connection);
}
