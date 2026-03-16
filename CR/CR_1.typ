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
    [Tommy-Verdi ORAVEC],
  )

= Mosaïque d'images avec subdivision de l'espace

Pour notre première version de la mosaïque, nous avons subdivisé récursivement l'image en fonction d'un seuil de variance (ici à 500).

Une fois le seuil de variance atteint, on sélectionne l'imagette ayant la moyenne de couleur la plus proche de la moyenne de la zone à remplacer. Nous avons utilisé la distance euclidienne dans l'espace RGB pour mesurer la proximité entre les couleurs.

---_Rajouter ici les infos sur la base de données d'imagettes utilisée_---


#figure(
rectangle([
  #[
    #set text(size: 11pt)
    #grid(
      columns: (auto, auto),
      rows: (auto, 1em),
      row-gutter: (1em),
      image("src.png", width: 100%),
      image("image.png", width: 100%),
      [Image originale],
      [Image mosaïque],
    )
 ]
], fill: silver, stroke: black),
caption: "On peut constater divers axes d'amélioration : une taille de grille minimum et une taille d'imagette maximale en fonction de la taille de l'image d'origine, ne pas réutiliser plusieurs fois la même imagettes et affiner la sélection d'imagettes"
)
\
