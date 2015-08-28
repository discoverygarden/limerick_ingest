<?php
/**
 * @file
 * A batch class file.
 */

/**
 * Batch preprocessor.
 */
class LimerickIngestBookBatch extends IslandoraBookBatch {

  /**
   * Static function to get object class.
   *
   * @see IslandoraBookBatch::getObjectClass()
   */
  protected static function getObjectClass() {
    return 'LimerickIngestBookBatchObject';
  }

  /**
   * Get main resource for objects.
   *
   * @see IslandoraBookBatch::scan()
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
   * @see IslandoraBookBatch::groupFiles()
   */
  protected function groupFiles($files) {
    $grouped = IslandoraScanBatch::groupFiles($files);
    foreach ($grouped as $name => &$name_to_file_object) {
      foreach ($name_to_file_object as $file_object) {
        $mods = limerick_ingest_get_metadata_file(
          $file_object->uri,
          $this->parameters['metadata_target'],
          $this->parameters['target']
        );
        if ($mods) {
          $name_to_file_object['.']['MODS'] = $mods;
        }
      }
    }
    return $grouped;
  }
}
