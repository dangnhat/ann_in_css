/**
 * @file acss_anim.h
 * @author  Nhat Pham  <phamhuudangnhat@gmail.com>.
 * @version 1.0
 * @date 04-Oct-2016
 * @brief This is header file for netAnim-related fucntions.
 */
 
#ifndef ACSS_ANIM_H_
#define ACSS_ANIM_H_

#include <string>
#include <stdint.h>

#include "ns3/netanim-module.h"

using namespace ns3;

/* Namespace */
namespace acss_anim_ns {
}

/* Class */
class acss_anim {

public:
  /**
   * @brief   Constructor.
   *          Create a new AnimationInterface object with output file name.
   *          Set background to a default one.
   *          Set mobility interval to 1s.
   *          Skip packet tracing.
   *
   * @param[in]   anim_file_name, file name for output NetAnim XML file.
   */
  acss_anim(const std::string anim_file_name);

  /**
   * @brief   Destructor.
   *          Delete AnimationInterface object.
   */
  ~acss_anim(void);

  /**
   * @brief   Update Primary Users icon to an object.
   *
   * @param[in]   node_id, Node id of the object.
   */
  void update_pu_icon(uint32_t node_id);

  /**
   * @brief   Update Secondary User icon to an object.
   *
   * @param[in]   node_id, Node id of the object.
   */
  void update_su_icon(uint32_t node_id);

  /**
   * @brief   Update Noise Generator icon to an object.
   *
   * @param[in]   node_id, Node id of the object.
   */
  void update_noise_gen_icon(uint32_t node_id);

private:
  AnimationInterface* anim_p;

  uint32_t pu_icon_id, su_icon_id, noise_gen_icon_id;
};

#endif /* ACSS_ANIM_H_ */
