macro_command main()
  unsigned short one_by_one[4] = {1,1,125,250}, worst_case[4] = {5,2,125,250}, sort_paper_model[4] = {1,2,125,250}, buff_paper_model[4] = {2,2,125,125}
  SetData(sort_paper_model[0], "Local HMI", RECIPE, "Avoti_paletesana.NumberOfRows")
  SetData(sort_paper_model[1], "Local HMI", RECIPE, "Avoti_paletesana.NumberOfColumns")
  SetData(sort_paper_model[2], "Local HMI", RECIPE, "Avoti_paletesana.SortBoxWidth")
  SetData(sort_paper_model[3], "Local HMI", RECIPE, "Avoti_paletesana.SortBoxLength")
  SetData(buff_paper_model[0], "Local HMI", "BufferNumberOfRows", 1)
  SetData(buff_paper_model[1], "Local HMI", "BufferNumberOfColumns", 1)
  SetData(buff_paper_model[2], "Local HMI", RECIPE, "Avoti_paletesana.BufferBoxWidth")
  SetData(buff_paper_model[3], "Local HMI", RECIPE, "Avoti_paletesana.BufferBoxLength")
end macro_command