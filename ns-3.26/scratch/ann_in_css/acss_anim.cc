/**
 * @file lte_anim.cc
 * @author  Nhat Pham  <phamhuudangnhat@gmail.com>.
 * @version 1.0
 * @date 04-Oct-2016
 * @brief This is source file for netAnim-related fucntions.
 */

#include "acss_anim.h"

using namespace ns3;

/* DEBUG */
#define LTE_ANIM_DEBUG_EN (1)
#include "acss_debug.h"

/* Configurations */
static const char pu_icon_path[] = "images/pu.png";
static const uint32_t pu_size = 2;

static const char su_icon_path[] = "images/su.png";
static const uint32_t su_size = 2;

static const char ng_icon_path[] = "images/noise.png";
static const uint32_t ng_size = 5;

/*----------------------------------------------------------------------------*/
acss_anim::acss_anim(const std::string anim_file_name) {
  anim_p = new AnimationInterface(anim_file_name);
  anim_p->EnablePacketMetadata(true);

  /* add resources */
  pu_icon_id = anim_p->AddResource(pu_icon_path);
  su_icon_id = anim_p->AddResource(su_icon_path);
  noise_gen_icon_id = anim_p->AddResource(ng_icon_path);
}

/*----------------------------------------------------------------------------*/
acss_anim::~acss_anim(void) {
  delete anim_p;
}

/*----------------------------------------------------------------------------*/
void acss_anim::update_pu_icon(uint32_t node_id) {
  //anim_p->UpdateNodeImage(node_id, pu_icon_id);
  anim_p->UpdateNodeColor(node_id, 255, 0, 0); /* Red */

  anim_p->UpdateNodeSize(node_id, pu_size, pu_size);
}

/*----------------------------------------------------------------------------*/
void acss_anim::update_su_icon(uint32_t node_id) {
  //anim_p->UpdateNodeImage(node_id, su_icon_id);
  anim_p->UpdateNodeColor(node_id, 0, 255, 0); /* Green */

  anim_p->UpdateNodeSize(node_id, su_size, su_size);
}

/*----------------------------------------------------------------------------*/
void acss_anim::update_noise_gen_icon(uint32_t node_id) {
  anim_p->UpdateNodeImage(node_id, noise_gen_icon_id);
  //anim_p->UpdateNodeColor(node_id, 0, 0, 255); /* Blue */

  anim_p->UpdateNodeSize(node_id, ng_size, ng_size);
}

