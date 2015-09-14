<?php
/**
 * @file
 * A batch class file.
 *
 * @todo: Metadata will concatenate the data's dirs into the file name.
 */

/**
 * Batch preprocessor.
 */
class LimerickIngestRecursiveBatch extends IslandoraScanBatch {

  /**
   * Static function to get object class.
   *
   * @see IslandoraScanBatch::getObjectClass()
   */
  protected static function getObjectClass() {
    return 'LimerickIngestRecursiveBatchObject';
  }

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
      // Prevent the use of strange directories.
      if ($value->name != '@eaDir') {
        $files[$value->name] = $value;
      }
    }
    return $files;
  }

  /**
   * Grab metadata DS.
   *
   * @see IslandoraScanBatch::groupFiles()
   */
  protected function groupFiles($files) {
    module_load_include('inc', 'limerick_ingest', 'includes/ingest');
    $grouped = parent::groupFiles($files);
    foreach ($grouped as $name => &$name_to_file_object) {
      foreach ($name_to_file_object as $file_object) {
        $mods = limerick_ingest_get_metadata_file(
          $file_object->uri,
          $this->parameters['metadata_target'],
          $this->parameters['target']
        );
        if ($mods) {
          $name_to_file_object['xml'] = $mods;
        }
      }
    }
    return $grouped;
  }
}
