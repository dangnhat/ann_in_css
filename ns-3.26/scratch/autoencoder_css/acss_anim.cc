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

/*----------------------------------------------------------------------------*/
lte_anim::lte_anim(const std::string anim_file_name) {
  anim_p = new AnimationInterface(anim_file_name);
  anim_p->EnablePacketMetadata(true);
}

/*----------------------------------------------------------------------------*/
lte_anim::~lte_anim(void) {
  delete anim_p;
}
