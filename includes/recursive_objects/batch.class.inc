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
      $files[$value->name] = $value;
    }
    return $files;
  }
}
