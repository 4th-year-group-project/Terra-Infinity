import dla_coalesce_parallel as cdla 
import dla
import image_producer as ip
import numpy as np
import matplotlib.pyplot as plt


#Produce plots which show the breakdown of CDLA compared
# to DLA at high particle densities

low_dla = ip.image_prod(256, 2500, dla)
low_cdla = ip.image_prod(256, 2500, cdla)

high_dla = ip.image_prod(256, 25000, dla)
high_cdla = ip.image_prod(256, 25000, cdla) 


# # ##Create a figure showing the images (2x2)
# fig, ax = plt.subplots(2, 2)
# ax[0, 0].imshow(low_dla)
# ax[0, 0].set_title("DLA")
# ax[0, 1].imshow(low_cdla)
# ax[0, 1].set_title("CDLA")
# ax[1, 0].imshow(high_dla)
# ax[1, 0].set_title("DLA")
# ax[1, 1].imshow(high_cdla)
# ax[1, 1].set_title("CDLA")

# ax[0, 0].set_xticks([])   
# ax[0, 0].set_yticks([])
# ax[0, 1].set_xticks([])
# ax[0, 1].set_yticks([])
# ax[1, 0].set_xticks([])
# ax[1, 0].set_yticks([])
# ax[1, 1].set_xticks([])
# ax[1, 1].set_yticks([])


# plt.show()

#Plot only high dla and cdla
fig, ax = plt.subplots(1, 2)
ax[0].imshow(high_dla)
ax[0].set_title("DLA")
ax[1].imshow(high_cdla)
ax[1].set_title("CDLA")
ax[0].set_xticks([])
ax[0].set_yticks([])
ax[1].set_xticks([])
ax[1].set_yticks([])
plt.show()


# low_dla = ip.image_prod(256, 10000, cdla)
# plt.imshow(low_dla)
# plt.show()