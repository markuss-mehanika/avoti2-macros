macro_command main()
  unsigned short rows, cols, box_width_mm, box_length_mm
  // TODO: might not be set, consider using GetDataEx
  GetData(rows, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  GetData(cols, "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  GetData(box_width_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  GetData(box_length_mm, "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  
  unsigned short default_width, default_length, one_by_one = 1, worst_case = 16, paper_model = 3
  float default_height = 100.0, half_default_height, tenth_default_height
  half_default_height = default_height/2.0
  tenth_default_height = default_height/10.0
  default_width = cols*box_width_mm
  default_length = rows*box_length_mm

  SetData(paper_model,          "Local HMI", RECIPE, "Avoti_paletesana.NumberOfLayers")
  // TODO: SortBoxHeight was moved to Sorting table, consider moving this code to set_grid_defaults
  SetData(default_height,       "Local HMI", RECIPE, "Avoti_paletesana.SortBoxHeight")
  SetData(default_height,       "Local HMI", RECIPE, "Avoti_paletesana.BufferBoxHeight")
  SetData(default_width,        "Local HMI", RECIPE, "Avoti_paletesana.PaletteWidth")
  SetData(default_length,       "Local HMI", RECIPE, "Avoti_paletesana.PaletteLength")
  SetData(half_default_height,  "Local HMI", RECIPE, "Avoti_paletesana.PaletteHeight")
  SetData(default_width,        "Local HMI", RECIPE, "Avoti_paletesana.SpacerWidth")
  SetData(default_length,       "Local HMI", RECIPE, "Avoti_paletesana.SpacerLength")
  SetData(tenth_default_height, "Local HMI", RECIPE, "Avoti_paletesana.SpacerHeight")
end macro_command