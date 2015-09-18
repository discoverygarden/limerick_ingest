<?php
/**
 * @file
 * A batch object class file.
 */

/**
 * Batch object.
 */
class LimerickIngestCompoundBatchObject extends IslandoraScanBatchObject {

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
      $directory_contents = file_scan_directory(
        $target_path,
        '/.*\.tif/',
        array('recurse' => FALSE)
      );
      ksort($directory_contents);
      foreach ($directory_contents as $uri => $value) {
        if ($value->name != '@eaDir') {
          $scaned[substr($uri, $target_length)] = $value;
        }
      }
      foreach ($scaned as $info) {
        $children[] = new LimerickIngestCompoundBatchObject($connection, NULL, array($info), $params);
      }
    }
    return $children;
  }

  /**
   * Add relationships.
   *
   * @see IslandoraScanBatchObject::addRelationships()
   */
  public function addRelationships() {
    if ($this->preprocessorParameters['content_models'] == array(ISLANDORA_COMPOUND_OBJECT_CMODEL)) {
      parent::addRelationships();
    }
    else {
      $this->addContentModelRelationships();
    }
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
}
