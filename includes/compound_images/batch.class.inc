<?php
/**
 * @file
 * A batch class file.
 */

/**
 * Batch preprocessor.
 */
class LimerickIngestCompoundIngestBatch extends IslandoraScanBatch {

  /**
   * Static function to get object class.
   *
   * @see IslandoraScanBatch::getObjectClass()
   */
  protected static function getObjectClass() {
    return 'LimerickIngestCompoundBatchObject';
  }

  /**
   * Get main resource for objects.
   *
   * @see IslandoraScanBatch::scan()
   */
  protected function scan() {
    $scaned = new stdClass();
    $scaned->uri = $this->parameters['target'];
    $scaned->ext = '';
    $scaned->name = pathinfo($scaned->uri, PATHINFO_FILENAME);
    $scaned->filename = pathinfo($scaned->uri, PATHINFO_BASENAME);
    return array($scaned);
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
