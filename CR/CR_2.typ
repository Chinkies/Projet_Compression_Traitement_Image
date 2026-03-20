#let header(title, color: red) = {
  set text(black, 30pt)
  set align(center+top)
  rect(
    fill: color,
    inset: 10pt,
    rect(
      fill: white,
      stroke: color,
      inset: 35pt,
      width: 100%,
      [#title],
    )
  )
}

#let exoTitre(number) = {
  set text(black, 20pt)
  rect(
    fill: aqua,
    stroke: aqua,
    inset: 10pt,
    width: 100%,
    [#"Exercice "#number],
  )
}

#let exEnonce(body) = {
  set text(15pt)
  rect(
    fill: silver,
    stroke: aqua,
    inset: 10pt,
    width: 100%,
    [#body],
  )
}

#let exo(number, body) = {
  stack(
    dir: ttb,
    exoTitre(number),
    exEnonce(body),
  )
}

#let rectangle(body, fill: silver, stroke: none) = {
  set text(black)
  set align(center)
  rect(
    fill: fill,
    stroke: stroke,
    inset: 2pt,
    radius: 4pt,
    [#body],
  )
}

#let solution(body) = {
  set text(black)
  set align(center)
  rect(
    fill: lime,
    inset: 20pt,
    radius: 4pt,
    [#body],
  )
}

#set page(
  margin: 5pt
)
#set text(15pt)
#header([Projet image : Mosaïque], color: gray)

#rect(
    fill: aqua,
    stroke: aqua,
    inset: 10pt,
    width: 100%,
    [Tommy-Verdi ORAVEC, Lucas PAULO, Victor ONIC],
  )

= Améliorations apportées à la mosaïque d'images

== Affinage de la sélection d'imagettes
Dans la version précédente nous procédions comme suit pour sélectionner une imagette : calcul de la moyenne de couleur de la zone à remplacer, puis sélection de l'imagette ayant la moyenne de couleur la plus proche.
Nous avons amélioré cette méthode en utilisant la distance euclidienne dans l'espace RGB pour mesurer séparément la proximité entre chaque composante RGB, ce qui nous a permis d'obtenir une mosaïque plus fidèle à l'image d'origine.

== Suppression de la redondance d'imagette
Dans la version précédente, aucune restriction n'était imposée sur le nombre de fois qu'une même imagette pouvait être utilisée, ce qui pouvait conduire à une mosaïque avec des zones très répétitives. Nous avons amélioré cela en limitant le nombre d'utilisations d'une même imagette.

== Augmentation de la taille de la base de données d'imagettes
La suppresion de la redondance des imagettes a rendu nécessaire l'augmentation de la taille de la base de données d'imagettes, afin d'avoir suffisamment d'imagettes différentes pour couvrir toute l'image d'origine. Nous avons donc ajouté de nombreuses imagettes à notre base de données, passant de 10 000 à 60 000 imagettes.
\
\
\
\
Pour les tests, nous avons utiliser une image de 3060x4080px, avec une taille d'imagette de 0.5% de la taille de l'image de l'image d'origine.

#figure(
rectangle([
  #[
    #set text(size: 11pt)
    #grid(
      columns: (auto, auto),
      rows: (auto, 1em, auto, 1em),
      row-gutter: (1em),
      image("src.png", width: 100%),
      image("test.png", width: 100%),
      [Image originale],
      [Image mosaïque],
      image("mosaique1_affPx.png", width: 100%),
      image("mosaique1_noRep.png", width: 100%),
      [Mosaique avec affinage de la sélection d'imagettes],
      [Mosaique avec suppression de la redondance d'imagettes],
    )
 ]
], fill: silver, stroke: black)
)
\
On constate une nette amélioration de la mosaique grâce à l'affinage de la sélection d'imagettes. Cependant, on peut voir quelques soucis avec certaines couleurs qui bavent un peu. La suppresion de la redondance réduit ce soucis, mais en apporte un autre : les couleurs deviennent de moins en moins fidèles à l'image d'origine, ceci est dû a une base de donnée encore trop petite malgrés son agrandissement, la mosaique utilisant environs 70% de la base de donnée d'imagettes, il faudrait encore l'agrandir pour obtenir une mosaique plus fidèle à l'image d'origine.