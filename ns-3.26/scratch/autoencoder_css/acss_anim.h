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
namespace lte_anim_ns {
}

/* Class */
class lte_anim {

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
  lte_anim(const std::string anim_file_name);

  /**
   * @brief   Destructor.
   *          Delete AnimationInterface object.
   */
  ~lte_anim(void);

private:
  AnimationInterface* anim_p;
};

#endif /* ACSS_ANIM_H_ */
