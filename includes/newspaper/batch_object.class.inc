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
  /**
   * Get the title for this object.
   *
   * Gets the label not empty; otherwise, grabs a title from the MODS.
   * Customized the location in MODS we retrieve the title from.
   *
   * @see IslandoraScanBatchObject::getTitle()
   */
  protected function getTitle() {
    if (!isset($this->label) || empty($this->label)) {
      // Acquire a title.
      $mods = $this->getMods();
      if ($mods) {
        $mods_doc = new DOMDocument();
        $mods_doc->loadXML($mods);
        $mods_xpath = new DOMXPath($mods_doc);
        $mods_xpath->registerNamespace('m', 'http://www.loc.gov/mods/v3');

        $this->label = $mods_xpath->evaluate('string(//m:mods/m:relatedItem/m:note/text())');
      }
    }
    return $this->label;
  }
}
