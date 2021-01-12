# Brute-Force-Belief-Change-Operator
Parses First-Order logic sentences and uses them to construct a completed operator for iterative belief change.

*******Vervollständigung von partiell spezifizierten Wissensänderungsoperatoren*******

Diese SW dient der Vervollständigung von partiell spezifizierten Wissensänderungsoperatoren.

--------------------------------------------------------------------------------------------------------------------

Input: 
Deterministischer endlicher Änderungsraum C über Ω und Formeln ϕ über einer FO-BC-Signatur, so dass C |= ϕ gilt.

Output: 
Ein vollständiger deterministischer endlicher Änderungsraum C über Ω, der C komplett enthält und für den C|= ϕ gilt.
Fehlermeldung, falls ein solcher Änderungsraum nicht existiert.

--------------------------------------------------------------------------------------------------------------------

How to use:
- Befüllen der Datei "INPUT_States.csv" mit den Zuständen inkl. Ordnungen des Eingabe-Änderungsraums
- Befüllen der Datei "INPUT_Edges.csv" mit den Kanten inkl. Alpha des Eingabe-Änderungsraums
- Befüllen der Datei "INPUT_FOBC.txt" mit den FOBC Bedingungen für die Vervollständigung
- Setzen der Optionen "reducedDiskSpace" und "outputMode" in der Datei "belief_change_space.h" und kompilieren des Codes
  Alternativ Wahl des entsprechendes Executables
- Ausführen des Programms im selben Verzeichnis wie die drei Input Dateien
- Ausgabedatei "completedOperator.dot" zur Visualisierung des vervollständigten Änderungsraums wird erstellt
- Überprüfung der erstellten Syntaxbäume und Fehlermeldungen in der Konsolenausgabe

--------------------------------------------------------------------------------------------------------------------

- Datei "INPUT_States.csv":	Zeilen, die mit einem Stern "*" beginnen, werden vom Programm als Kommentarzeilen ignoriert.
				Die erste Spalte muss mit den Namen der vordefinierten Zustände gefüllt werden. 
				Die erste Zeile muss mit den Namen der der betrachteten Welten gefüllt werden. Das Programm 
				definiert aus dieser Zeile die Anzahl an Welten.
				Die so aufgespannte Matrix aus den Namen der Zuständen und Namen der Welten muss vollständig 
				mit natürlichen Zahlen inkl. 0 befüllt werden. Diese repräsentieren den Plausibilitätsindex
				der jeweiligen Welt, wobei 0 am plausibelsten ist (Modelle).
				Jede Zeile der Matrix definiert somit einen Zustand mit zugehöriger Ordnung des Änderungsraums.
				
- Datei "INPUT_Edges.csv":	Zeilen, die mit einem Stern "*" beginnen, werden vom Programm als Kommentarzeilen ignoriert.
				Jede Zeile steht für eine vordefinierte Kante des Änderungsraums.
				Die erste Spalte muss mit den Namen der Ausgangszustände der Kanten gefüllt werden. 
				Die zweite Spalte muss mit den Namen der Zielzustände der Kanten gefüllt werden.
				Die angegebenen Namen der betrachteten Welten muss (auch in der Reihenfolge) mit der ersten
				Zeile der "INPUT_States.csv" übereinstimmen.
				Die so aufgespannte Matrix aus den Namen der Ausgangs-/Zielzustände und der Namen der Welten
				muss vollständig mit Werten aus jeweils "true" oder "false" befüllt werden, je nachdem ob die
				jeweilige Welt Modell von der Information der Kante ist. Jede Zeile muss mindestens ein "true"
				enthalten, da neue Informationen ohne Modelle nicht akzeptiert werden.
				Es ist darauf zu achten, dass nur Namen für Zustände verwendet werden, welche vorher in der 
				"INPUT_States.csv" Datei definiert wurden. 

- Datei "INPUT_FOBC.txt":	Zeilen, die mit einem Stern "*" beginnen, werden vom Programm als Kommentarzeilen ignoriert.
				Folgende Operatoren stehen zur Verfügung für die Formeln: &, |, ~, =>, <=>, A(x), E(x).
				Folgende Prädikate stehen zur Verfügung: 
 					- Mod(x) -> WAHR, wenn x ∈ Mod(α)
 					- TPO1(x,y) -> WAHR, wenn x ≤ y in Quasiordnung des Ausgangszustandes der Kante
					- TPO2(x,y) -> WAHR, wenn x ≤ y in Quasiordnung des Ausgangszustandes der Kante
				Die Konstante "c" kann verwendet werden, sowie Variablen [u, v,..., z]. Die Variablen können
				außerdem noch mit bis zu zwei Nummern erweitert werden, z.B. u1, x15, w79. 
				Folgende Bindungsprioritäten sind festgelegt: (stark) ~, ∧, ∨, =>, <=>, ∀, ∃ (schwach)
				Es kann eine beliebige Anzahl an Formeln eingegeben werden. Leerzeichen werden vom Algorithmus
				automatisch entfernt.

- Option "reducedDiskSpace":	= false --> totale Quasiordnungen zur Vervollständigung der Zustände im Änderungsraum werden
					CPU-effizient, d.h. schneller generiert und verarbeitet durch einmalige Generierung
					einer Tabelle durch baumartige Suche. Benötigt allerdings potentiell viel Speicherplatz
					bei hoher Anzahl von Welten.
				= true --> totale Quasiordnungen werden jeweils wenn benötigt im Algorithmus neu generiert.
					Dadurch entfällt der evtl. große Speicherbedarf für eine Tabelle. Dafür verlangsamt
					sich die Ausführung.

- Option "outputMode":		= "single" --> Der Brute-Force Algorithmus betreibt die Suche nach einem neuen Zielzustand
					für eine fehlende Kante immer chronologisch und bricht bei der ersten gefundenen
					Kante ab.
				= "random" --> Der Brute-Force Algorithmus wählt zufällige Zustände als nächstzuprüfend aus
					und bricht ebenfalls bei der ersten gefundenen Kante ab.
				= "all" --> Der Brute-Force Algorithmus sucht alle möglichen Kanten und fügt sie dem Änderungs-
					raum hinzu. In diesem Modus wird in der Konsolenausgabe die Anzahl an möglichen
					vollständigen Änderungsräumen für die Eingabe ausgegeben.

- Datei "completedOperator.dot":	Die erstellte Datei im .dot Format zeigt den erstellten Änderungsraum.
					Vordefinierte Zustände und Kanten sind dabei blau gefärbt und besitzen ihren
					vordefinierten Namen. Vom Algorithmus hinzugefügte Zustände und Kanten sind grau
					hinterlegt. Zustände besitzen Namen "s0*, s1*, ....". Die Welten besitzen die vordefinierten
					Namen aus den Eingabedateien. Die Visualisierung des .dot Formats ist beispielsweise hier
					online möglich: </https://dreampuf.github.io/GraphvizOnline/>

- Konsolenausgabe:		Das Programm fängt einige Fehler bei der Eingabe ab. In diesem Fall wird die Ausführung meist
				trotzdem fortgesetzt und die fehlerhafte Eingabe ignoriert. Deshalb ist es wichtig die Fehler-
				meldungen zu prüfen. Außerdem wird eine Repräsentation der aufgestellten Syntaxbäume ausgegeben.
				Mit Hilfe dieser kann nochmal geprüft werden, ob die FOBC Eingaben korrekt geparst wurden.

--------------------------------------------------------------------------------------------------------------------

  (c)	Marco Stock, 06.10.20
  Masterarbeit, Studiengang: Praktische Informatik
	Fernuniversität in Hagen
	Betreuer: Prof. Dr. Christoph Beierle

