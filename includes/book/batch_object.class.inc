<?php
/**
 * @file
 * A batch object class file.
 */

/**
 * Batch object.
 */
class LimerickIngestBookBatchObject extends IslandoraBookBookBatchObject {

  /**
   * Function getChildren.
   *
   * @see IslandoraBookBookBatchObject::getChildren()
   */
  public function getChildren(IslandoraTuque $connection) {
    // Create page objects, return in an array.
    $children = array();

    $scaned = array();
    $directory_contents = file_scan_directory(
      drupal_realpath($this->preprocessorParameters['target']),
      '/.*\.tif/',
      array('recurse' => FALSE)
    );
    ksort($directory_contents);
    $key = 1;
    foreach ($directory_contents as $uri => $value) {
      if ($value->name != '@eaDir') {
        $scaned[$key++]['.']['OBJ'] = $value;
      }
    }
    foreach ($scaned as $sequence => $info) {
      if ($sequence != '.') {
        $children[] = new IslandoraBookPageBatchObject(
          $connection,
          $this->id,
          $sequence,
          $info,
          $this->preprocessorParameters
        );
      }
    }

    return $children;
  }
}
