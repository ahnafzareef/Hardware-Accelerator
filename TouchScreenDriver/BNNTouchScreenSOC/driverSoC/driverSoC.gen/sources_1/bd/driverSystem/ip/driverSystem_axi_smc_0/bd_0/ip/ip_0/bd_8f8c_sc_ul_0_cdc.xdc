## (c) Copyright 2024, Advanced Micro Devices, Inc.
## 
## Permission is hereby granted, free of charge, to any person obtaining a 
## copy of this software and associated documentation files (the "Software"), 
## to deal in the Software without restriction, including without limitation 
## the rights to use, copy, modify, merge, publish, distribute, sublicense, 
## and/or sell copies of the Software, and to permit persons to whom the 
## Software is furnished to do so, subject to the following conditions:
## 
## The above copyright notice and this permission notice shall be included in 
## all copies or substantial portions of the Software.
## 
## THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
## IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
## FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
## THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
## LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
## FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
## DEALINGS IN THE SOFTWARE.
#############################################################################

  ### Waiving for CDC-11 as same wdata is synchronized in multiple cdc modules if same asynchronous clock is used on multiple MI's
  #set_false_path -quiet -from [get_pins -filter REF_PIN_NAME=~*C -of_objects [get_cells -hierarchical -filter {NAME =~ *gen_axi_full_nxm.sasd_inst/gen_write.m_wdata_reg*}]]
      create_waiver -type CDC -id {CDC-11} -user "axi_switch" -desc "AXI Switch resets contain known-good CDC paths" -tags "1171415" -scope -internal \
          -from [get_pins -quiet -filter REF_PIN_NAME=~*C -of_objects [get_cells -hierarchical -filter {NAME =~ *gen_axi_full_nxm.sasd_inst/gen_write.m_wdata_reg*}]]

#  set_false_path -quiet -to [get_pins -filter REF_PIN_NAME=~*D -of_objects [get_cells -hierarchical -filter {NAME =~ *swbd_psr/*syncstages_ff_reg[0]}]]
#  create_waiver -type CDC -id {CDC-11} -user "axi_switch" -desc "AXI Switch resets contain known-good CDC paths" -tags "1171415" -scope -internal \
#      -to [get_pins -quiet -filter REF_PIN_NAME=~*D -of_objects [get_cells -hierarchical -filter {NAME =~ *swbd_psr/*syncstages_ff_reg[0]}]]
