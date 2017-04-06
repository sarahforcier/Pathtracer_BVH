Path Tracer Episode VI: Return of the Acceleration Structures
======================

Sarah Forcier 
58131867


Cornell Box
-------------
BVH Build Time: 0 milliseconds

| Integrator | BVH Render | without | speedup |
| -----------| ---------- | ------- | -------
| Full 		 | 429401 | 569307 | 1.33
| Direct 	 | 83374 | 117069 | 1.40
| Naive 	 | 175994 | 256472 | 1.46

Mario Box
--------------
BVH Build Time: 51 milliseconds

| Integrator | BVH Render | without | speedup |
| -----------| ---------- | ------- | -------
| Direct 	 | 6097 |  1174644 | 193


Wolf with a Tree
-----------
BVH Build Time: 26 milliseconds

Cat on a Box
-----------
BVH Build Time: 26 milliseconds

400 samples per pixel and 512x512 resolution. 
