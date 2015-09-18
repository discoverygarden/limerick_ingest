<?php
/**
 * @file
 * A batch class file.
 */

/**
 * Batch preprocessor.
 */
class LimerickIngestImageBatch extends IslandoraScanBatch {
  /**
   * Get main DS for objects.
   *
   * @see IslandoraScanBatch::scan()
   */
  protected function scan() {
    $scaned = $this->scanDirectory($this->parameters['target']);
    $files = array();
    foreach ($scaned as $key => $value) {
      // Prevent the use of strange directories.
      if (strpos($value->uri, '@eaDir') === FALSE) {
        $files[$key] = $value;
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
    $grouped = parent::groupFiles($files);
    ksort($grouped);
    foreach ($grouped as $name => &$name_to_file_object) {
      foreach ($name_to_file_object as $file_object) {
        $mods = limerick_ingest_get_metadata_file(
          $file_object->uri,
          $this->parameters['metadata_target'],
          $this->parameters['target']
        );
        if (!isset($first_mods)) {
          if ($mods) {
            $first_mods = $mods;
          }
          else {
            $first_mods = FALSE;
          }
        }
        if ($mods) {
          $name_to_file_object['xml'] = $mods;
        }
        elseif ($first_mods) {
          $name_to_file_object['xml'] = $first_mods;
        }
      }
    }
    return $grouped;
  }
}
