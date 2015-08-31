<?php
/**
 * @file
 * Main ingest functions.
 */

/**
 * Route ingests based on type.
 */
function limerick_ingest_ingest($type, $parameters, $connection) {
  if (!isset($connection)) {
    $connection = islandora_get_tuque_connection();
  }
  switch ($type) {
    case 'compound_images':
      $parameters['content_models'] = array(ISLANDORA_COMPOUND_OBJECT_CMODEL);
      $preprocessor = new LimerickIngestCompoundIngestBatch($connection, $parameters);
      break;

    case 'book':
      $parameters['wait_for_metadata'] = FALSE;
      $parameters['directory_dedup'] = FALSE;
      $parameters['content_models'] = array('islandora:bookCModel');
      $preprocessor = new LimerickIngestBookBatch($connection, $parameters);
      break;

    case 'images':
      $parameters['content_models'] = array('islandora:sp_large_image_cmodel');
      $preprocessor = new LimerickIngestImageBatch($connection, $parameters);
      break;

    case 'recursive_objects':
      $preprocessor = new LimerickIngestRecursiveBatch($connection, $parameters);
      break;

    case 'issues':
      $parameters['generate_ocr'] = TRUE;
      $parameters['parent_relationship_pred'] = 'isMemberOf';
      $parameters['wait_for_metadata'] = FALSE;
      $parameters['directory_dedup'] = FALSE;
      $parameters['content_models'] = array('islandora:newspaperIssueCModel');
      $parameters['type'] = 'directory';
      $preprocessor = new LimerickIngestIssuesBatch($connection, $parameters);
      break;

    case 'postcards':
      $parameters['content_models'] = array(ISLANDORA_COMPOUND_OBJECT_CMODEL);
      $preprocessor = new LimerickIngestPostcardBatch($connection, $parameters);
      break;

  }
  islandora_batch_handle_preprocessor($preprocessor);
}

/**
 * Get info on a metadata file for a base file from a directory.
 */
function limerick_ingest_get_metadata_file($base_file, $metadata_dir, $data_dir, $type = FALSE) {
  if (!$type) {
    $base_file = realpath($base_file);
  }
  $metadata_dir = realpath($metadata_dir);
  $data_dir = realpath($data_dir);
  $base_file_after_dir = substr($base_file, strlen($data_dir) + 1, strlen($base_file));
  if (!$base_file_after_dir) {
    $base_dir = strrchr($base_file, '/');
    $base_file_after_dir = substr($base_dir, 1, strlen($base_dir));
  }
  $base_file_replaced_slashes = str_replace('/', '_', $base_file_after_dir);
  $metadata_base = pathinfo($base_file_replaced_slashes, PATHINFO_FILENAME) . '.xml';
  $posible_metadata_uri = "$metadata_dir/$metadata_base";
  if (is_file($posible_metadata_uri)) {
    $metadata_uri = $posible_metadata_uri;
  }
  else {
    $metadata_base = str_replace('-', '_', $metadata_base);
    $metadata_directory_contents = file_scan_directory(
      $metadata_dir,
      '/.*\.xml/',
      array('recurse' => FALSE)
    );
    foreach ($metadata_directory_contents as $uri => $value) {
      if (substr($uri, -strlen($metadata_base), strlen($uri)) == $metadata_base) {
        $metadata_uri = $uri;
        break;
      }
    }
  }

  if (isset($metadata_uri)) {
    $metadata_info = new stdClass();
    $metadata_info->name = pathinfo($metadata_uri, PATHINFO_FILENAME);
    $metadata_info->filename = $metadata_base;
    $metadata_info->uri = $metadata_uri;
    $metadata_info->ext = 'xml';
    return $metadata_info;
  }
  return FALSE;
}
