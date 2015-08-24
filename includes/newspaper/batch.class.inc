<?php
/**
 * @file
 * A batch class file.
 */

/**
 * Batch preprocessor.
 */
class LimerickIngestIssuesBatch extends IslandoraNewspaperBatch {

  /**
   * Get main DS for objects.
   *
   * @see IslandoraScanBatch::scan()
   */
  protected function scan() {
    $files = array();
    $directory_contents = file_scan_directory(
      drupal_realpath($this->parameters['target']),
      $this->getPattern(),
      array('recurse' => FALSE)
    );
    $key = 1;
    foreach ($directory_contents as $uri => $value) {
      $files[$value->name][] = $value;
    }
    return $files;
  }

  /**
   * Static function to get object class.
   *
   * @see IslandoraNewspaperBatch::getObjectClass()
   */
  protected static function getObjectClass() {
    return 'LimerickIngestNewspaperBatchObject';
  }

  /**
   * Grab metadata DS.
   *
   * @see IslandoraScanBatch::groupFiles()
   */
  protected function groupFiles($files) {
    ksort($files);
    foreach ($files as $name => &$name_to_file_object) {
      foreach ($name_to_file_object as $file_object) {
        $mods = limerick_ingest_get_metadata_file(
          $file_object->uri,
          $this->parameters['metadata_target'],
          $this->parameters['target'],
          TRUE
        );
        if ($mods) {
          $name_to_file_object['.']['MODS'] = $mods;
        }
      }
    }
    return $files;
  }
}
