macro_command main()
  unsigned short a
  float b
  unsigned short rows, cols, box_width_mm, box_length_mm
  // TODO: might not be set, consider using GetDataEx
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  
  a = 1
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  b = 100.0
  SetData(b, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")
  a = cols*box_width_mm
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.PaletteWidth")
  a = rows*box_length_mm
  SetData(a, "Local HMI", RECIPE, "Avoti_paletesana.PaletteLength")
  b = 100.0
  SetData(b, "Local HMI", RECIPE, "Avoti_paletesana.PaletteHeight")
end macro_command