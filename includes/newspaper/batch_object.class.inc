<?php
/**
 * @file
 * A batch object class file.
 */

/**
 * Batch object.
 */
class LimerickIngestNewspaperBatchObject extends IslandoraNewspaperIssueBatchObject {

  /**
   * Discover pages.
   *
   * @see IslandoraNewspaperIssueBatchObject::getChildren()
   */
  public function getChildren(IslandoraTuque $connection) {
    // Create page objects, return in an array.
    $children = array();
    $scaned = array();
    $directory_contents = file_scan_directory(
      $this->objectInfo[0]->uri,
      '/.*\.tif/',
      array('recurse' => FALSE)
    );
    ksort($directory_contents);
    $key = 1;
    foreach ($directory_contents as $uri => $value) {
      $scaned[$key++]['.']['OBJ'] = $value;
    }
    foreach ($scaned as $sequence => $info) {
      $children[] = new IslandoraNewspaperPageBatchObject(
        $connection,
        $this->id,
        $sequence,
        $info,
        $this->preprocessorParameters
      );
    }
    return $children;
  }
}
