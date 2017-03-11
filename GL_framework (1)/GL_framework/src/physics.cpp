#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>
#include <GL\glew.h>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <cstdio>
#include <time.h>
bool show_test_window = false;

int emm = 100;
float life = 1;
int solver;
float elastic = 0;
float friction = 0;
float sphereRad = 1;
int fontOrWaterfall;
int changedFont;
int changedSolver;
const float gravity = -9.81;
float accX = 0;
float accZ = 0;

float fx = 0;
float fy = -1;
float fz = 0;
float mass = 1;

int var1;
int var2;

int part = emm/4;

float cool = 0;
struct Particle
{
	
	glm::vec3 pos;
	glm::vec3 velocity;
	float lifetime;
	glm::vec3 antPos;
	glm::vec3 nextPos;
};


namespace LilSpheres {
	extern const int maxParticles;
	extern void setupParticles(int numTotalParticles, float radius = 0.05f);
	extern void cleanupParticles();
	extern void updateParticles(int startIdx, int count, float* array_data);
	extern void drawParticles(int startIdx, int count);
	
}

namespace Sphere {
	extern void setupSphere(glm::vec3 pos = glm::vec3(0.f, 1.f, 0.f), float radius = 1.f);
	extern void cleanupSphere();
	extern void updateSphere(glm::vec3 pos, float radius = 1.f);
	extern void drawSphere();
}

Particle* totalParts;
float *partVerts;

void GUI() {
	Sphere::updateSphere(glm::vec3(0.f, 1.f, 0.f), sphereRad);
	{	
		
			
		//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//TODO
		ImGui::Separator();
		ImGui::Text("Solver Methods:");
		//Euler & Verlet Buttons
		ImGui::RadioButton("Euler Solver", &solver, 0); ImGui::SameLine();
		ImGui::RadioButton("Verlet Solver", &solver, 1);

		ImGui::Text("Emitter Type:");
		//Font or Waterfall	
		ImGui::RadioButton("Font", &fontOrWaterfall, 0); ImGui::SameLine();
		ImGui::RadioButton("Waterfall", &fontOrWaterfall, 1);
		//Emission & life Sliders
		ImGui::SliderInt("Emission Rate", &emm, 100, 10000);
		ImGui::SliderFloat("Life Expectancy", &life, 1, 5);
		
		//Elastic & Friccion Inputs
		ImGui::SliderFloat("Elastic Coeficient", &elastic, 0, 1);
		//ImGui::InputFloat("Elastic Coeficient", &elastic);
		ImGui::InputFloat("Friction Coeficient", &friction);
		//Sphere Position & radius
		ImGui::SliderFloat("Sphere Radius", &sphereRad, 0, 5);
		//ImGui::InputFloat("Sphere Radius", &sphereRad);
		
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void PhysicsInit() {
	emm = 100;
	part = emm / 4;
	srand(time(NULL));
	//TODO
	//printf("%d", fontOrWaterfall);
	totalParts = new Particle[LilSpheres::maxParticles];
	LilSpheres::setupParticles(LilSpheres::maxParticles);
	partVerts = new float[LilSpheres::maxParticles * 3];
	if (solver == 0) {
		if (fontOrWaterfall == 0) {
			for (int i = 0; i < LilSpheres::maxParticles; ++i) {

				totalParts[i].pos.x = 0;
				totalParts[i].pos.y = 2;
				totalParts[i].pos.z = 0;
				partVerts[i * 3 + 0] = totalParts[i].pos.x;
				partVerts[i * 3 + 1] = totalParts[i].pos.y;
				partVerts[i * 3 + 2] = totalParts[i].pos.z;

				totalParts[i].velocity.x = ((float)rand() / RAND_MAX) * 5 - 2.5f;
				totalParts[i].velocity.y = ((float)rand() / RAND_MAX) * 5 + 6;
				totalParts[i].velocity.z = ((float)rand() / RAND_MAX) * 5 - 2.5f;

				totalParts[i].lifetime = life;

			}

		}
		else if (fontOrWaterfall == 1) {

			for (int i = 0; i < LilSpheres::maxParticles; ++i) {
				totalParts[i].pos = glm::vec3(5, 5, ((float)rand() / RAND_MAX) *3.f);
				partVerts[i * 3 + 0] = totalParts[i].pos.x;
				partVerts[i * 3 + 1] = totalParts[i].pos.y;
				partVerts[i * 3 + 2] = totalParts[i].pos.z;

				totalParts[i].velocity.x = ((float)rand() / RAND_MAX) *-2 - 0.25;
				totalParts[i].velocity.y = ((float)rand() / RAND_MAX);
				totalParts[i].velocity.z = 0;


				totalParts[i].lifetime = life;
			}
			LilSpheres::updateParticles(0, LilSpheres::maxParticles, partVerts);
		}
		changedFont = fontOrWaterfall;
	}
	else if (solver == 1)
	{
		if (fontOrWaterfall == 0) {
			for (int i = 0; i < LilSpheres::maxParticles; ++i) {

				totalParts[i].pos.x = 0;
				totalParts[i].pos.y = 2;
				totalParts[i].pos.z = 0;
				partVerts[i * 3 + 0] = totalParts[i].pos.x;
				partVerts[i * 3 + 1] = totalParts[i].pos.y;
				partVerts[i * 3 + 2] = totalParts[i].pos.z;
				totalParts[i].antPos = glm::vec3(totalParts[i].pos.x, totalParts[i].pos.y - 0.1, totalParts[i].pos.z);
				totalParts[i].velocity.x = ((float)rand() / RAND_MAX) * 5 - 2.5f;
				totalParts[i].velocity.y = ((float)rand() / RAND_MAX) * 5 + 6;
				totalParts[i].velocity.z = ((float)rand() / RAND_MAX) * 5 - 2.5f;

				totalParts[i].lifetime = life;

				
			}

		}
		else if (fontOrWaterfall == 1) {

			for (int i = 0; i < LilSpheres::maxParticles; ++i) {
				totalParts[i].pos = glm::vec3(5, 5, ((float)rand() / RAND_MAX) *3.f);
				partVerts[i * 3 + 0] = totalParts[i].pos.x;
				partVerts[i * 3 + 1] = totalParts[i].pos.y;
				partVerts[i * 3 + 2] = totalParts[i].pos.z;

				totalParts[i].velocity.x = ((float)rand() / RAND_MAX) *-2;
				totalParts[i].velocity.y = ((float)rand() / RAND_MAX);
				totalParts[i].velocity.z = 0;


				totalParts[i].lifetime = life;
				totalParts[i].antPos = glm::vec3(0, 2 - (totalParts[i].velocity.y*(1000 / 30)), 0);
			}

		}
		changedFont = fontOrWaterfall;
	}
	changedSolver = solver;
}
void PhysicsUpdate(float dt) {
	//TODO
	var1 = 0;
	var2 = part -10;

	if (part >= emm) {
		part = emm;
	}
	if (changedSolver != solver) {
		PhysicsInit();
	}
	if (fontOrWaterfall != changedFont) {
		PhysicsInit();
	}
	//EulerSolver
	if (solver == 0) {
		//FONT
		if (fontOrWaterfall == 0) {
			for (int i = 0; i < part; ++i) {
				totalParts[i].lifetime -= dt;
				if (totalParts[i].lifetime <= 0) {
					if (fontOrWaterfall == 0) {
						totalParts[i].pos.x = 0;
						totalParts[i].pos.y = 2;
						totalParts[i].pos.z = 0;
						partVerts[i * 3 + 0] = totalParts[i].pos.x;
						partVerts[i * 3 + 1] = totalParts[i].pos.y;
						partVerts[i * 3 + 2] = totalParts[i].pos.z;

						totalParts[i].velocity.x = ((float)rand() / RAND_MAX) * 5 - 2.5f;
						totalParts[i].velocity.y = ((float)rand() / RAND_MAX) * 5+6;
						totalParts[i].velocity.z = ((float)rand() / RAND_MAX) * 5 - 2.5f;

						totalParts[i].lifetime = life;

					}
				}
			}


			for (int i = 0; i < part; ++i) {

				totalParts[i].pos.x += (dt * totalParts[i].velocity.x) + (0.5 * (gravity * (dt * dt)));
				totalParts[i].velocity.x += (accX*dt);
				
				totalParts[i].pos.y += (dt * totalParts[i].velocity.y) + (0.5 * (gravity * (dt * dt)));
				totalParts[i].velocity.y += (gravity*dt);
				
				totalParts[i].pos.z += (dt * totalParts[i].velocity.z) + (0.5 * (gravity * (dt * dt)));
				totalParts[i].velocity.z += (accZ*dt);
				partVerts[i * 3 + 0] = totalParts[i].pos.x;
				partVerts[i * 3 + 1] = totalParts[i].pos.y;
				partVerts[i * 3 + 2] = totalParts[i].pos.z;

				if (totalParts[i].pos.y <= 0.25 || totalParts[i].pos.y >= 10) {
					totalParts[i].velocity.y = -totalParts[i].velocity.y*elastic;
				}
				if (totalParts[i].pos.x <= -5 || totalParts[i].pos.x >= 5) {
					totalParts[i].velocity.x = -totalParts[i].velocity.x*elastic;
				}
				if (totalParts[i].pos.z <= -5 || totalParts[i].pos.z >= 5) {
					totalParts[i].velocity.z = -totalParts[i].velocity.z*elastic;
				}

				if (glm::distance(glm::vec3(0.f, 1.f, 0.f), totalParts[i].pos) - sphereRad <= 0) {
					totalParts[i].velocity.y = -totalParts[i].velocity.y*elastic;
					totalParts[i].velocity.x += totalParts[i].velocity.x;
					totalParts[i].velocity.z += totalParts[i].velocity.z;
				}
			}
			/*for (int i = part; i < LilSpheres::maxParticles; ++i) {
				totalParts[i].pos.x = 0;
				totalParts[i].pos.y = 2;
				totalParts[i].pos.z = 0;
				partVerts[i * 3 + 0] = totalParts[i].pos.x;
				partVerts[i * 3 + 1] = totalParts[i].pos.y;
				partVerts[i * 3 + 2] = totalParts[i].pos.z;

				totalParts[i].velocity.x = ((float)rand() / RAND_MAX) * 5 - 2.5f;
				totalParts[i].velocity.y = ((float)rand() / RAND_MAX) * 10;
				totalParts[i].velocity.z = ((float)rand() / RAND_MAX) * 5 - 2.5f;

				totalParts[i].lifetime = life;
			}*/
			part += 10;
			printf("%d \n", part);
			LilSpheres::updateParticles(0, part, partVerts);
		}
		//Waterfall
		else if (fontOrWaterfall == 1) {
			for (int i = 0; i < part; ++i) {
				totalParts[i].lifetime -= dt;
				if (totalParts[i].lifetime <= 0) {

					if (fontOrWaterfall == 1)
					{
						totalParts[i].pos = glm::vec3(5, 5, ((float)rand() / RAND_MAX) *3.f);
						partVerts[i * 3 + 0] = totalParts[i].pos.x;
						partVerts[i * 3 + 1] = totalParts[i].pos.y;
						partVerts[i * 3 + 2] = totalParts[i].pos.z;

						totalParts[i].velocity.x = ((float)rand() / RAND_MAX) *-2 -0.25;
						totalParts[i].velocity.y = ((float)rand() / RAND_MAX);
						totalParts[i].velocity.z = 0;


						totalParts[i].lifetime = life;
					}
				}
			}

			for (int i = 0; i < part; ++i) {


				totalParts[i].pos.x += (dt * totalParts[i].velocity.x) + (0.5 * (gravity * (dt * dt)));
				totalParts[i].velocity.x += (accX*dt);
				if (totalParts[i].pos.y >= 0.25) {
					totalParts[i].pos.y += (dt * totalParts[i].velocity.y) + (0.5 * (gravity * (dt * dt)));
					totalParts[i].velocity.y += (gravity*dt);
				}
				totalParts[i].pos.z += (dt * totalParts[i].velocity.z) + (0.5 * (gravity * (dt * dt)));
				totalParts[i].velocity.z += (accZ*dt);
				partVerts[i * 3 + 0] = totalParts[i].pos.x;
				partVerts[i * 3 + 1] = totalParts[i].pos.y;
				partVerts[i * 3 + 2] = totalParts[i].pos.z;

				if (totalParts[i].pos.y <= 0.25) {
					totalParts[i].velocity.y = -totalParts[i].velocity.y*elastic;

					//totalParts[i].velocity.x += totalParts[i].velocity.x;
					//totalParts[i].velocity.z += totalParts[i].velocity.z;
				}
				if (glm::distance(glm::vec3(0.f, 1.f, 0.f), totalParts[i].pos) - sphereRad <= 0) {
					//printf("XOCA");
					totalParts[i].velocity.y = -totalParts[i].velocity.y*elastic;

					totalParts[i].velocity.x += totalParts[i].velocity.x;
					totalParts[i].velocity.z += totalParts[i].velocity.z;
				}


			}
			part += 10;
			LilSpheres::updateParticles(0, emm, partVerts);
		}
	}
	//Verlet
	else if (solver == 1) {
		//Font
		if (fontOrWaterfall == 0) {
			for (int i = 0; i < part; ++i) {
				totalParts[i].lifetime -= dt;
				if (totalParts[i].lifetime <= 0) {
						totalParts[i].pos.x = 0;
						totalParts[i].pos.y = 2;
						totalParts[i].pos.z = 0;
						partVerts[i * 3 + 0] = totalParts[i].pos.x;
						partVerts[i * 3 + 1] = totalParts[i].pos.y;
						partVerts[i * 3 + 2] = totalParts[i].pos.z;
						totalParts[i].antPos = glm::vec3(totalParts[i].pos.x, totalParts[i].pos.y - 0.1, totalParts[i].pos.z);
						totalParts[i].velocity.x = ((float)rand() / RAND_MAX) * 5 - 2.5f;
						totalParts[i].velocity.y = ((float)rand() / RAND_MAX) * 5 + 6;
						totalParts[i].velocity.z = ((float)rand() / RAND_MAX) * 5 - 2.5f;

						totalParts[i].lifetime = life;					
				}

				for (int i = 0; i < part; ++i) {

					glm::vec3 temp = totalParts[i].pos;


					totalParts[i].pos.x = totalParts[i].pos.x + (totalParts[i].pos.x - totalParts[i].antPos.x) + (fx/mass)*(dt*dt);
					totalParts[i].pos.y = totalParts[i].pos.y + (totalParts[i].pos.y - totalParts[i].antPos.y) + (fy / mass)*(dt*dt);
					totalParts[i].pos.z = totalParts[i].pos.z + (totalParts[i].pos.z - totalParts[i].antPos.z) + (fz / mass)*(dt*dt);

					
					
					totalParts[i].antPos = temp;


					partVerts[i * 3 + 0] = totalParts[i].pos.x;
					partVerts[i * 3 + 1] = totalParts[i].pos.y;
					partVerts[i * 3 + 2] = totalParts[i].pos.z;
				}
			}
		}
		part += 10;
		LilSpheres::updateParticles(0, part, partVerts);
	}
}
void PhysicsCleanup() {
	//TODO
	LilSpheres::cleanupParticles();
	delete[] totalParts;
	delete[] partVerts;
}