<?php
/**
 * @file
 * A batch object class file.
 */

/**
 * Batch object.
 */
class LimerickIngestPostcardBatchObject extends IslandoraScanBatchObject {

  /**
   * Get compound children.
   *
   * @see IslandoraScanBatchObject::getChildren()
   */
  public function getChildren(IslandoraTuque $connection) {
    $children = array();
    if ($this->preprocessorParameters['content_models'] == array(ISLANDORA_COMPOUND_OBJECT_CMODEL)) {
      $params = $this->preprocessorParameters;
      $params['content_models'] = array('islandora:sp_large_image_cmodel');
      $params['parent'] = $this->id;
      $params['parent_relationship_pred'] = 'isConstituentOf';
      $params['parent_relationship_uri'] = FEDORA_RELS_EXT_URI;

      $scaned = array();
      $target_path = drupal_realpath($params['target']);
      $target_length = strlen($target_path) + 1;
      $base_name = substr($this->objectInfo['tif']->name, 0, strlen($this->objectInfo['tif']->name) - 1);
      $directory_contents = file_scan_directory(
        $target_path,
        "/$base_name.\.tif/",
        array('recurse' => FALSE)
      );
      ksort($directory_contents);
      foreach ($directory_contents as $uri => $value) {
        $scaned[substr($uri, $target_length)] = $value;
      }
      foreach ($scaned as $info) {
        $children[] = new LimerickIngestPostcardBatchObject($connection, NULL, array($info), $params);
      }
    }
    return $children;
  }
  /**
   * Add an object to the compound.
   */
  public function makeCompound() {
    if (!($this->preprocessorParameters['content_models'] == array(ISLANDORA_COMPOUND_OBJECT_CMODEL))) {
      module_load_include('inc', 'islandora_compound_object', 'includes/manage.form');
      islandora_compound_object_add_parent($this, $this->preprocessorParameters['parent']);
    }
  }

  /**
   * Function batch_process.
   *
   * Overriden to handle compoundness.
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
        if (!($this->preprocessorParameters['content_models'] == array(ISLANDORA_COMPOUND_OBJECT_CMODEL) &&
          strrchr($obj->filename, '.') == '.tif')) {
          list($dsid, $mimetype) = static::determineDSIDAndMimetype($obj->filename);
          $obj_datastream = $this->constructDatastream($dsid);
          $obj_datastream->mimetype = $mimetype;
          $obj_datastream->label = "$dsid Datastream";
          $obj_datastream->setContentFromFile($obj->uri, FALSE);

          $this->ingestDatastream($obj_datastream);
        }
      }
    }
    $this->makeCompound();
    return ISLANDORA_BATCH_STATE__DONE;
  }

  /**
   * Determine the datastream ID for the given file.
   *
   * Overriden to not use statics.
   *
   * @see IslandoraScanBatchObject::determineDSIDAndMimetype()
   */
  protected function determineDSIDAndMimetype($name) {
    $mime_detect = NULL;
    $dsids = NULL;

    if ($mime_detect === NULL || $dsids === NULL) {
      $mime_detect = new MimeDetect();
      module_load_include('inc', 'islandora', 'includes/utilities');
      $models = (array) $this->preprocessorParameters['content_models'];
      $dsids = islandora_get_datastreams_requirements_from_models($models);
    }

    $mimetype = $mime_detect->getMimetype($name);
    $dsid = FALSE;
    // Determine which stream this should be... Uses the first matching, as
    // received from the DC-COMPOSITE.
    foreach ($dsids as $ds => $info) {
      if (in_array($mimetype, $info['mime'])) {
        $dsid = $ds;
        break;
      }
    }
    return array($dsid, $mimetype);
  }
}
