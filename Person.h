/*********************************************************************/
/* Filename: PotatoHead.h                                            */
/* The implementation of the PotatoHead class, which uses spheres to */
/* represent the various "body parts" of a simplified 3D model of    */
/* Mr. Potato-Head.                                                  */
/*********************************************************************/




#ifndef PERSON_H
#define PERSON_H

//#define glutSolidSphere glutWireSphere

#include "Graphics.h"
#include "Graphics.Animation.h"
#include "Graphics.Range.h"
#include "Graphics.Material.h"

using namespace Graphics;
using namespace Graphics::AnimationLibrary;

class Person
{
private:
  float SIZE_OF_HEAD;
  float SIZE_OF_NOSE;
  float SIZE_OF_NECK;
  float SIZE_OF_TORSO;
  float SIZE_OF_LEG;
  float SIZE_OF_KNEE;
  float SIZE_OF_ANKLE;
  float SIZE_OF_FOOT;
  float SIZE_OF_ELBOW;
  
  int FRAMES_PER_ANIMATION;

  Material<>* material_skin;

  Animation* upper_left_arm_animation;
  Animation* upper_right_arm_animation;
  Animation* lower_left_arm_animation;
  Animation* lower_right_arm_animation;
  Animation* upper_torso_animation;
  Animation* pelvis_animation;
  Animation* upper_left_leg_animation;
  Animation* upper_right_leg_animation;
  Animation* lower_left_leg_animation;
  Animation* lower_right_leg_animation;
  Animation* head_animation;
  Animation* left_foot_animation;
  Animation* right_foot_animation;

  Animation* walk_animation;
  float walk_position;
  float walk_distance;

  float upper_right_arm_angle;
  float upper_left_arm_angle;
  float lower_right_arm_angle;
  float lower_left_arm_angle;
  float upper_torso_angle;
  float pelvis_angle;
  float upper_left_leg_angle;
  float upper_right_leg_angle;
  float lower_left_leg_angle;
  float lower_right_leg_angle;
  float head_angle;
  float left_foot_angle;
  float right_foot_angle;

  Range<>* upper_arm_range;
  Range<>* lower_arm_range;
  Range<>* upper_torso_range;
  Range<>* pelvis_range;
  Range<>* upper_leg_range;
  Range<>* lower_leg_range;
  Range<>* head_range;
  Range<>* foot_range;


public:
  enum Side
  {
    Left,
    Right
  };
  
  Person()
  {
    this->FRAMES_PER_ANIMATION = 15;

    this->material_skin = new Material<>();
    this->material_skin->set_ambient( 1.0f, 1.0f, 0.0f, 1.0f );
    this->material_skin->set_diffuse( 1.0f, 1.0f, 0.0f, 1.0f );
    this->material_skin->set_specular( 1.0f, 1.0f, 1.0f, 1.0f );
    this->material_skin->set_shininess( 400.0f );

    this->upper_left_arm_angle  = 0.0f;
    this->upper_right_arm_angle = 0.0f;
    this->lower_left_arm_angle  = 0.0f;
    this->lower_right_arm_angle = 0.0f;
    this->upper_torso_angle     = 0.0f;
    this->pelvis_angle          = 0.0f;
    this->upper_left_leg_angle  = 0.0f;
    this->upper_right_leg_angle = 0.0f;
    this->lower_left_leg_angle  = 0.0f;
    this->lower_right_leg_angle = 0.0f;
    this->head_angle            = 0.0f;
    this->left_foot_angle       = 0.0f;
    this->right_foot_angle      = 0.0f;

    this->upper_arm_range   = new Range<>( -8.0f, 8.0f );
    this->lower_arm_range   = new Range<>( 1.0f, 15.0f );
    this->upper_torso_range = new Range<>( -2.5f, 2.5f );
    this->pelvis_range      = new Range<>( -2.5f, 2.5f );
    this->upper_leg_range   = new Range<>( -15.0f, 15.0f );
    this->lower_leg_range   = new Range<>( -25.0f, 0.0f );
    this->head_range        = new Range<>( -45.0f, 45.0f );
    this->foot_range        = new Range<>( -25.0f, 25.0f );

    this->walk_position  = 0.0f;
    this->walk_distance  = 1.0f;
    this->walk_animation = new Animation( this->walk_position, this->walk_distance, this->FRAMES_PER_ANIMATION ); 

    this->upper_left_arm_animation  = new Animation( this->upper_left_arm_angle,  this->upper_arm_range->max, this->FRAMES_PER_ANIMATION );
    this->upper_right_arm_animation = new Animation( this->upper_right_arm_angle, this->upper_arm_range->min, this->FRAMES_PER_ANIMATION );
    this->lower_left_arm_animation  = new Animation( this->lower_left_arm_angle,  this->lower_arm_range->max, this->FRAMES_PER_ANIMATION );
    this->lower_right_arm_animation = new Animation( this->lower_right_arm_angle, this->lower_arm_range->min, this->FRAMES_PER_ANIMATION );
    this->upper_torso_animation     = new Animation( this->upper_torso_angle,     this->upper_torso_range->max, this->FRAMES_PER_ANIMATION );
    this->pelvis_animation          = new Animation( this->pelvis_angle,          this->pelvis_range->min, this->FRAMES_PER_ANIMATION );
    this->upper_left_leg_animation  = new Animation( this->upper_left_leg_angle,  this->upper_leg_range->min, this->FRAMES_PER_ANIMATION );
    this->upper_right_leg_animation = new Animation( this->upper_right_leg_angle, this->upper_leg_range->max, this->FRAMES_PER_ANIMATION );
    this->lower_left_leg_animation  = new Animation( this->lower_left_leg_angle,  this->lower_leg_range->min, this->FRAMES_PER_ANIMATION );
    this->lower_right_leg_animation = new Animation( this->lower_right_leg_angle, this->lower_leg_range->max, this->FRAMES_PER_ANIMATION );
    this->head_animation            = new Animation( this->head_angle,            this->head_range->max, this->FRAMES_PER_ANIMATION * 2 );
    this->left_foot_animation       = new Animation( this->left_foot_angle,       this->foot_range->min, this->FRAMES_PER_ANIMATION );
    this->right_foot_animation      = new Animation( this->right_foot_angle,      this->foot_range->max, this->FRAMES_PER_ANIMATION );
    
    SIZE_OF_HEAD  = 1.0f;
    SIZE_OF_NOSE  = 0.1f;
    SIZE_OF_NECK  = 0.5f;
    SIZE_OF_TORSO = 1.0f;
    SIZE_OF_LEG   = 0.9f;
    SIZE_OF_KNEE  = 0.6f;
    SIZE_OF_ANKLE = 0.4f;
    SIZE_OF_FOOT  = 1.0f;
    SIZE_OF_ELBOW = 0.5f;
  }

  ~Person(){}

  void animate()
  {
    this->upper_left_arm_animation->animate_range( this->upper_arm_range, Quadratic::ease_in_and_out );
    this->upper_right_arm_animation->animate_range( this->upper_arm_range, Quadratic::ease_in_and_out );

    this->lower_left_arm_animation->animate_range( this->lower_arm_range, Quadratic::ease_in_and_out );
    this->lower_right_arm_animation->animate_range( this->lower_arm_range, Quadratic::ease_in_and_out );

    this->upper_torso_animation->animate_range( this->upper_torso_range, Quadratic::ease_in_and_out );
    this->pelvis_animation->animate_range( this->pelvis_range, Quadratic::ease_in_and_out );

    this->upper_left_leg_animation->animate_range( this->upper_leg_range, Quadratic::ease_in_and_out );
    this->upper_right_leg_animation->animate_range( this->upper_leg_range, Quadratic::ease_in_and_out );

    this->lower_left_leg_animation->animate_range( this->lower_leg_range, Quadratic::ease_in_and_out );
    this->lower_right_leg_animation->animate_range( this->lower_leg_range, Quadratic::ease_in_and_out );

    this->head_animation->animate_range( this->head_range, Quadratic::ease_in_and_out );

    this->left_foot_animation->animate_range( this->foot_range, Quadratic::ease_in_and_out );
    this->right_foot_animation->animate_range( this->foot_range, Quadratic::ease_in_and_out );
  }

  /*void walk()
  {
    if( !this->walk_animation->is_animating() )
      this->walk_animation->reset( this->walk_position + this->walk_distance, this->FRAMES_PER_ANIMATION );
    
    this->walk_animation->animate( Linear::tween );
  }*/

  void draw()
  {
    this->animate();
    //this->walk();

    glPushMatrix();
      material_skin->apply();

      glTranslatef( 0.0f, 0.0f, this->walk_position );

      glScalef( 0.175f, 0.175f, 0.175f );
      //glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );

      this->draw_lower_torso();
    glPopMatrix();
  }

  void draw_head()
  {
    glPushMatrix();
      glTranslatef( 0.0f, 1.0f, 0.0f );

      this->draw_nose();

      glutSolidSphere( 1.0f, 3, 3 );
      //glutSolidSphere( 1.0f, 30, 30 );
    glPopMatrix();
  }

  void draw_nose()
  {
    glPushMatrix();
      glTranslatef( 0.0f, 0.0f, 1.05f );

      glScalef( 0.1f, 0.1f, 0.1f );
      glRotatef( 90.0f, 0.0f, 1.0f, 0.0f );

      glutSolidSphere( 1.0f, 3, 3 );
      //glutSolidSphere( 1.0f, 10, 10 );
    glPopMatrix();
  }

  void draw_neck()
  {
    glPushMatrix();
      glTranslatef( 0.0f, 1.0f, 0.0f );

      this->draw_head();

      glScalef( 0.6f, 0.1f, 0.6f );
      glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );

      glutSolidSphere( 1.0f, 3, 3 );
      //glutSolidSphere( 1.0f, 15, 3 );
    glPopMatrix();
  }

  void draw_upper_torso()
  {
    glPushMatrix();
	  glTranslatef( 0.0f, 1.5f, 0.0f );

      glRotatef( -this->upper_left_arm_angle, 2.0f, 0.0f, 0.0f );
      this->draw_upper_arm( Left );
      glRotatef( this->upper_left_arm_angle, 1.0f, 0.0f, 0.0f );

      glRotatef( -this->upper_right_arm_angle, 2.0f, 0.0f, 0.0f );
      this->draw_upper_arm( Right );
      glRotatef( this->upper_right_arm_angle, 1.0f, 0.0f, 0.0f );

	  glRotatef( -this->head_angle, 0.0f, 2.0f, 0.0f );
      this->draw_neck();
	  glRotatef( this->head_angle, 0.0f, 1.0f, 0.0f );

      glScalef( 2.0f, 1.0f, 1.0f );

      glutSolidSphere( 1.0f, 3, 3 );
      //glutSolidSphere( 1.0f, 30, 30 );
    glPopMatrix();
  }

  void draw_upper_arm( Side side )
  {
    glPushMatrix();
      glTranslatef( ((side == Left) ? 1 : -1 ) * 1.9f, -1.0f, 0.0f );

      this->draw_elbow( side );

      glScalef( 0.6f, 1.2f, 0.6f );

      glutSolidSphere( 1.0f, 3, 3 );
      //glutSolidSphere( 1.0f, 15, 15 );
    glPopMatrix();
  }

  void draw_elbow( Side side )
  {
    float angle;
    if( side == Left )
      angle = this->lower_left_arm_angle;
    else
      angle = this->lower_right_arm_angle;

    glPushMatrix();
      glTranslatef( 0.0f, -1.25f, 0.0f );

      glRotatef( -angle, 2.0f, 0.0f, 0.0f );
      this->draw_lower_arm();
      glRotatef( angle, 1.0f, 0.0f, 0.0f );

      glScalef( 0.5f, 0.45f, 0.5f );

      glutSolidSphere( 1.0f, 3, 3 );
      //glutSolidSphere( 1.0f, 10, 10 );
    glPopMatrix();
  }

  void draw_lower_arm()
  {
    glPushMatrix();
      glTranslatef( 0.0f, -1.0f, 0.0f );

      this->draw_hand();

      glScalef( 0.5f, 1.0f, 0.5f );

      glutSolidSphere( 1.0f, 3, 3 );
      //glutSolidSphere( 1.0f, 15, 15 );
    glPopMatrix();
  }

  void draw_hand()
  {
    glPushMatrix();
      glTranslatef( 0.0f, -1.25f, 0.0f );

      glScalef( 0.4f, 1.0f, 0.7f );

      glutSolidSphere( SIZE_OF_ELBOW, 10, 10 );
    glPopMatrix();
  }
  void draw_lower_torso()
  {
    glPushMatrix();
      glTranslatef( 0.0f, -SIZE_OF_TORSO * 1.5, 0.0f );

      glRotatef( -this->upper_torso_angle, 0.0f, 0.0f, 2.0f );
	  draw_upper_torso();
      glRotatef( this->upper_torso_angle, 0.0f, 0.0f, 1.0f );

      glRotatef( -this->pelvis_angle, 0.0f, 0.0f, 2.0f );
      draw_pelvis();
      glRotatef( this->pelvis_angle, 0.0f, 0.0f, 1.0f );

      glScalef( 1.5f, 2.0f, 1.25f );
      glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );

      glutSolidSphere( SIZE_OF_TORSO, 15, 15 );
    glPopMatrix();
  }

  void draw_pelvis()
  {
    glPushMatrix();
      glTranslatef( 0.0f, -2.0f, 0.0f );

      glRotatef( -this->upper_right_leg_angle, 2.0f, 0.0f, 0.0f );
      draw_upper_leg( Right );
      glRotatef( this->upper_right_leg_angle, 1.0f, 0.0f, 0.0f );

      glRotatef( -this->upper_left_leg_angle, 2.0f, 0.0f, 0.0f );
      glRotatef( this->pelvis_angle, 0.0f, 0.0f, 2.0f );
      draw_upper_leg( Left );
      glRotatef( -this->pelvis_angle, 0.0f, 0.0f, 1.0f );
      glRotatef( this->upper_left_leg_angle, 1.0f, 0.0f, 0.0f );

      glScalef( 1.25f, 1.0f, 1.0f );

      glutSolidSphere( SIZE_OF_TORSO, 10, 10 );
    glPopMatrix();
  }

  void draw_upper_leg( Side side )
  {
    glPushMatrix();
      glTranslatef( ((side == Left) ? 1 : -1 ) * SIZE_OF_LEG, -1.5f, 0.0f );

      draw_knee( side );

      glScalef( 0.8f, 1.5f, 0.8f );

      glutSolidSphere( SIZE_OF_LEG, 20, 20 );
    glPopMatrix();
  }

  void draw_knee( Side side )
  {
    float angle;
    if( side == Left )
      angle = this->lower_left_leg_angle;
    else
      angle = this->lower_right_leg_angle;

    glPushMatrix();
      glTranslatef( 0.0f, -1.25f, 0.0f );

      glRotatef( -angle, 2.0f, 0.0f, 0.0f );
      draw_lower_leg( side );
      glRotatef( angle, 1.0f, 0.0f, 0.0f );

      glutSolidSphere( SIZE_OF_KNEE, 10, 10 );
    glPopMatrix();
  }

  void draw_lower_leg( Side side )
  {
    glPushMatrix();
      glTranslatef( 0.0f, -1.0f, 0.0f );

      draw_ankle( side );

      glScalef( 0.7f, 1.5f, 0.7f );

      glutSolidSphere( SIZE_OF_LEG, 30, 30 );
    glPopMatrix();
  }

  void draw_ankle( Side side )
  {
    float angle;
    if( side == Left )
      angle = this->left_foot_angle;
    else
      angle = this->right_foot_angle;

    glPushMatrix();
      glTranslatef( 0.0f, -1.25f, 0.0f );

      glRotatef( -angle, 1.0f, 0.0f, 0.0f );
      draw_foot();

      glutSolidSphere( SIZE_OF_ANKLE, 10, 10 );
    glPopMatrix();
  }

  void draw_foot()
  {
    glPushMatrix();
      glTranslatef( 0.0f, -SIZE_OF_ANKLE, SIZE_OF_FOOT/2 );

      glScalef( 0.7f, 0.3f, 1.0f );

      glutSolidSphere( SIZE_OF_FOOT, 10, 10 );
    glPopMatrix();
  }

};

#define glutSolidSphere glutSolidSphere

#endif
