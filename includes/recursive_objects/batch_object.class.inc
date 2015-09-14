<?php
/**
 * @file
 * A batch object class file.
 */

/**
 * Batch object.
 */
class LimerickIngestRecursiveBatchObject extends IslandoraScanBatchObject {
  protected static $modelMap = array(
    'pdf' => 'islandora:sp_pdf',
    'tif' => 'islandora:sp_large_image_cmodel',
    'mp3' => 'islandora:sp-audioCModel',
    'wav' => 'islandora:sp-audioCModel',
    '' => 'islandora:collectionCModel',
  );

  /**
   * Recurse through the directories.
   *
   * @see IslandoraScanBatchObject::getChildren()
   */
  public function getChildren(IslandoraTuque $connection) {
    $children = array();
    $info = reset($this->objectInfo);
    if (!is_dir(drupal_realpath($info->uri))) {
      return $children;
    }
    $scaned = array();

    $params = $this->preprocessorParameters;
    $params['parent'] = $this->id;

    $directory_contents = file_scan_directory(
      drupal_realpath($info->uri),
      '/.*/',
      array('recurse' => FALSE)
    );
    ksort($directory_contents);
    foreach ($directory_contents as $uri => $value) {
      // Prevent the use of strange directories.
      if ($value->name != '@eaDir') {
        $scaned[$uri] = $value;
      }
    }
    foreach ($scaned as $uri => $info) {
      $mods = limerick_ingest_get_metadata_file(
        $info->uri,
        $params['metadata_target'],
        $params['target']
      );
      $info_array = array($info);
      if ($mods) {
        $info_array['xml'] = $mods;
      }
      $children[$uri] = new LimerickIngestRecursiveBatchObject(
        $connection,
        $info->name,
        $info_array,
        $params
      );
    }
    return $children;
  }

  /**
   * Add relationships.
   *
   * @see IslandoraScanBatchObject::addRelationships()
   */
  public function addRelationships() {
    $info = reset($this->objectInfo);
    $ext = pathinfo($info->filename, PATHINFO_EXTENSION);
    $cmodel = self::$modelMap[$ext];
    $this->preprocessorParameters['content_models'] = array($cmodel);
    parent::addRelationships();
  }

  /**
   * Function batch_process.
   *
   * Overriden to handle directories.
   *
   * @see IslandoraScanBatchObject::batchProcess()
   */
  public function batchProcess() {
    // Use object_info to create some datastreams.
    $this->label = $this->getTitle();
    $this->getMods();
    $this->getDc();

    if (!isset($this['OBJ'])) {
      $other = array_diff_key($this->objectInfo, array_flip($this->objectInfoDatastreamExclusions));
      foreach ($other as $obj) {
        if (!is_dir($obj->uri)) {
          list($dsid, $mimetype) = static::determineDSIDAndMimetype($obj->filename);
          $obj_datastream = $this->constructDatastream('OBJ');
          $obj_datastream->mimetype = $mimetype;
          $obj_datastream->label = "$dsid Datastream";
          $obj_datastream->setContentFromFile($obj->uri, FALSE);

          $this->ingestDatastream($obj_datastream);
        }
      }
    }

    return ISLANDORA_BATCH_STATE__DONE;
  }
}
