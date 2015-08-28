<?php
/**
 * @file
 * A batch class file.
 */

/**
 * Batch preprocessor.
 */
class LimerickIngestPostcardBatch extends IslandoraScanBatch {

  /**
   * Static function to get object class.
   *
   * @see IslandoraScanBatch::getObjectClass()
   */
  protected static function getObjectClass() {
    return 'LimerickIngestPostcardBatchObject';
  }

  /**
   * Only make one compound per an A/B set.
   *
   * @see IslandoraScanBatch::scan()
   */
  protected function scan() {
    $scaned = $this->scanDirectory($this->parameters['target']);
    $a_filter = function($value) {
      return (strrchr($value->name, 'A') == 'A');
    };
    return array_filter($scaned, $a_filter);
  }

  /**
   * Grab metadata DS.
   *
   * @see IslandoraScanBatch::groupFiles()
   */
  protected function groupFiles($files) {
    $grouped = parent::groupFiles($files);
    foreach ($grouped as $name => &$name_to_file_object) {
      foreach ($name_to_file_object as $file_object) {
        $mods = limerick_ingest_get_metadata_file(
          substr($file_object->uri, 0, strlen($file_object->uri) - 5),
          $this->parameters['metadata_target'],
          $this->parameters['target'],
          TRUE
        );
        if ($mods) {
          $name_to_file_object['xml'] = $mods;
        }
      }
    }
    return $grouped;
  }
}
