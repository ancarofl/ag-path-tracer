# ag-path-tracer

CPU Path Tracer developed in January 2019 as the final assignment of the Advanced Graphics 2018-2019 course at UU. The PT was built on top of a ray tracing framework using a BVH as an acceleration structure, previously developed for the same course.

<b> Currently supported features: </b><br />
-Primitves: plane, sphere, triangle, triangle mesh <br />
-Materials: Lambert, pure specular, dielectrics, Phong <br /> 
-Importance Sampling (Cosine Weighted Indirect Illumination and Next Event Estimation) <br /> 
-Russian Roulette <br /> 
-Filtering <br /> 
-(basic) Depth of Field <br /> 
-(basic) Skybox <br />

<b> Some 512x512 scenes rendered using the framework: </b><br />
Cornell box <br />
![Cornell box](https://i.imgur.com/sMqih6L.png)<br />
Snowmen scene <br />
![Snowmen scene](https://i.imgur.com/D4QGRfq.png)<br />

<b> Branches: </b><br />
<b> master: </b> stable CPU path tracer version which was submitted. <br />
<b> master-rt: </b> stable previous version of the project; Whitted-style ray tracer + BVH. <br />
<b> development: </b> current development branch. <br />
