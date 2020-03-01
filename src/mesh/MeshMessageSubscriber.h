/*
 * MeshMessageSubscriber.h
 *
 *  Created on: Feb 19, 2020
 *      Author: evsejho
 */

#ifndef SRC_MESH_MESHMESSAGESUBSCRIBER_H_
#define SRC_MESH_MESHMESSAGESUBSCRIBER_H_

namespace mesh {

class MeshMessageSubscriber {
public:
	virtual void msg_recv() = 0;
	virtual ~MeshMessageSubscriber() {}
};

} /* namespace mesh */

#endif /* SRC_MESH_MESHMESSAGESUBSCRIBER_H_ */
