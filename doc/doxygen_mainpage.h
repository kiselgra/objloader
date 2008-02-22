/*!	\author Kai
 * 	\mainpage objloader Beschreibung
 *
 * ObjLoader ist ein Interface für Alias/Wavefront Obj Datei Loader.
 *
 * 	Nach den einlesen eines Vertex, einer Texturkoordinate oder einer Normalen wird direkt die entsprechende
 * 	Funktion aufgerufen. Ebenso wird für jeden Teilausdruck innerhalb einer face-definition AddFaceNode
 * 	aufgerufen. Nachdem alle Knotenpunkte der Oberfläche eingelesen wurden wird FaceDone aufgerufen.
 * 	Das Material funktioniert analog.
 *
 * 	Die einzelnen Funktionen müssen vom Anwender überschrieben werden. So gibts das Parsen umsonst, aber
 * 	ohne gleichzeitig Datenstrukturen aufzuzwängen.
 */

