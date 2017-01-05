#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef NULL
#define NULL 0
#endif

#define PI 3.141592653589793
#define STEP 0.00001

typedef struct {
	double x;
	double y;
} Vec;

double dist(double x1, double y1, double x2, double y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

Vec* mux(Vec* a, Vec* b) {
	Vec* ptr = (Vec*)malloc(sizeof(Vec));
	ptr->x = a->x * b->x;
	ptr->y = a->y * b->y;
	return ptr;
}

double dot(Vec* a, Vec* b) {
	return a->x * b->x + a->y * b->y;
}

double ang(Vec* a, Vec* b) {
	return acos(dot(a, b) / (dist(0.0, 0.0, a->x, a->y) * dist(0.0, 0.0, b->x, b->y)));
}

double arg(Vec* v) {
	return atan(v->y / v->x);
}

Vec aVec = { -1.0, -1.0 };

typedef struct _surface {
	double x;
	double r;
	double n;
	struct _surface* next;
} Surface;

typedef struct {
	double x;
	double y;
	double angle;
} Light;

Surface* createSurface(double x, double r, double n) {
	Surface* ptr = (Surface*)malloc(sizeof(Surface));
	ptr->x = x;
	ptr->r = r;
	ptr->n = n;
	ptr->next = NULL;
	return ptr;
}

char reachedSurface(Light* light, Surface* surface) {
	if (abs(dist(light->x, light->y, surface->x + surface->r, 0.0) - abs(surface->r)) > STEP) {
		light->x += STEP * cos(light->angle);
		light->y += STEP * sin(light->angle);
		return 0;
	} else {
		return 1;
	}
}

void transSurface(Light* light, Surface* surface, double pn) {
	Vec _nVec = { light->x - surface->x - surface->r, light->y };
	Vec* nVec = &_nVec;
	Vec lVec = { cos(light->angle), sin(light->angle) };
	if (surface->r > 0.0) nVec = mux(nVec, &aVec);
	double i = ang(&lVec, nVec);
	double r = asin(sin(i) * pn / surface->n);
	double nAng = arg(nVec);
	if (arg(&lVec) > nAng) {
		light->angle = nAng + r;
	} else {
		light->angle = nAng - r;
	}
}

void onUpdate(Light* self, Surface* surface) {
	Light* l = self; Surface* s = surface;
	double pn = 1.0;
	while (1) {
		//printf("[DEBUG] x = %lf, \ty = %lf, \ta = %lf\n", l->x, l->y, l->angle);
		if (reachedSurface(l, s)) {
			transSurface(l, s, pn);
			if (s->next == NULL) {
				while (fabs(l->y) > STEP) {
					l->x += STEP * cos(l->angle);
					l->y += STEP * sin(l->angle);
				}
				break;
			} else {
				pn = s->n;
				s = s->next;
			}
		}
	}
}

Light* createLight(double x, double y, double angle) {
	Light* ptr = (Light*)malloc(sizeof(Light));
	ptr->x = x;
	ptr->y = y;
	ptr->angle = angle;
	return ptr;
}

void showLight(char* head, Light* light) {
	printf("%s: \tx = %lf, y = %lf, a = %lf\n", head, light->x, light->y, light->angle / PI * 180.0);
}

void main() {
	double K9 = 1.516374, ZF1 = 1.647664;
	Light* l = createLight(0.0, 12.5, 0.0 / 180 * PI);
	Light* t = createLight(0.0, -12.5, 0.0 / 180 * PI);
	Surface* s = createSurface(0, 82.2, K9);
	s->next = createSurface(5.5, -57.81, ZF1);
	s->next->next = createSurface(5.5 + 3.0, -4742, 1.0);
	s->next->next->next = createSurface(5.5 + 3.0 + 2.8, 71.45, K9);
	s->next->next->next->next = createSurface(5.5 + 3.0 + 2.8 + 3.5, 65535.0, 1.0);
	onUpdate(l, s);
	onUpdate(t, s);

	printf("\nResult:\n");
	showLight("Light 1", l);
	showLight("Light 2", t);
	system("pause");
}
