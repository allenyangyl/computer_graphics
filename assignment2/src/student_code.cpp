/*
 * Student solution for CMU 15-462 Project 2 (MeshEdit)
 *
 * Implemented by ____ on ____.
 *
 */

#include "student_code.h"
#include "mutablePriorityQueue.h"

namespace CMU462
{
   VertexIter HalfedgeMesh::splitEdge( EdgeIter e0 )
   {
      // TODO This method should split the given edge and return an iterator to the newly inserted vertex.
      // TODO The halfedge of this vertex should point along the edge that was split, rather than the new edges.

            // assign existed elements
			HalfedgeIter h00 = e0 -> halfedge();
            HalfedgeIter h10 = h00 -> twin();
            // return if boundary
            if(h00 -> isBoundary() || h10 -> isBoundary()){
                return VertexIter();
            }
            HalfedgeIter h01 = h00 -> next();
            HalfedgeIter h02 = h01 -> next();
            HalfedgeIter h11 = h10 -> next();
            HalfedgeIter h12 = h11 -> next();
            FaceIter f00 = h00 -> face();
            FaceIter f10 = h10 -> face();
            VertexIter v00 = h01 -> vertex();
            VertexIter v01 = h02 -> vertex();
            VertexIter v10 = h11 -> vertex();
            VertexIter v11 = h12 -> vertex();

            // allocate new elements
            VertexIter v = newVertex();
            v -> position = (v00 -> position + v10 -> position) / 2;
            FaceIter f01 = newFace();
            FaceIter f11 = newFace();
            EdgeIter e1 = newEdge();
            EdgeIter e2 = newEdge();
            EdgeIter e3 = newEdge();
            HalfedgeIter h03 = newHalfedge();
            HalfedgeIter h04 = newHalfedge();
            HalfedgeIter h05 = newHalfedge();
            HalfedgeIter h13 = newHalfedge();
            HalfedgeIter h14 = newHalfedge();
            HalfedgeIter h15 = newHalfedge();

            // Set neighbors
            // half edge
            h00 -> setNeighbors(h01, h10, v, e0, f00);
            h01 -> setNeighbors(h03, h01 -> twin(), v00, h01 -> edge(), f00);
            h03 -> setNeighbors(h00, h04, v01, e1, f00);
            h05 -> setNeighbors(h04, h15, v10, e3, f01);
            h04 -> setNeighbors(h02, h03, v, e1, f01);
            h02 -> setNeighbors(h05, h02 -> twin(), v01, h02 -> edge(), f01);
            h10 -> setNeighbors(h13, h00, v00, e0, f10);
            h13 -> setNeighbors(h12, h14, v, e2, f10);
            h12 -> setNeighbors(h10, h12 -> twin(), v11, h12 -> edge(), f10);
            h15 -> setNeighbors(h11, h05, v, e3, f11);
            h11 -> setNeighbors(h14, h11 -> twin(), v10, h11 -> edge(), f11);
            h14 -> setNeighbors(h15, h13, v11, e2, f11);
            // face
            if(f00 -> halfedge() == h02){
                f00 -> setNeighbors(h03);
            }
            if(f10 -> halfedge() == h11){
                f10 -> setNeighbors(h13);
            }
            f01 -> setNeighbors(h02);
            f11 -> setNeighbors(h11);
            // vertex
            if(v10 -> halfedge() == h00){
                v10 -> setNeighbors(h05);
            }
            v -> setNeighbors(h00); v -> isNew = true;
            // edge
            e1 -> setNeighbors(h03); e1 -> isNew = true;
            e2 -> setNeighbors(h14); e2 -> isNew = true;
            e3 -> setNeighbors(h05); e3 -> isNew = e0 -> isNew;

            return v;
	 }

   VertexIter HalfedgeMesh::collapseEdge( EdgeIter e )
   {
      // TODO This method should collapse the given edge and return an iterator to the new vertex created by the collapse.

            // assign existed elements
            HalfedgeIter h0 = e -> halfedge();
            HalfedgeIter h1 = h0 -> twin();
            HalfedgeIter h00 = h0 -> next();
            HalfedgeIter h01 = h00 -> next();
            HalfedgeIter h10 = h1 -> next();
            HalfedgeIter h11 = h10 -> next();
            HalfedgeIter h20 = h10 -> twin();
            HalfedgeIter h21 = h01 -> twin();
            HalfedgeIter h30 = h20 -> next() -> next();
            HalfedgeIter h31 = h21 -> next() -> next();
            HalfedgeIter h40 = h00 -> twin();
            HalfedgeIter h41 = h11 -> twin();
            EdgeIter e0 = h10 -> edge();
            EdgeIter e1 = h01 -> edge();
            FaceIter f0 = h0 -> face();
            FaceIter f1 = h1 -> face();
            VertexIter v00 = h00 -> vertex();
            VertexIter v01 = h01 -> vertex();
            VertexIter v10 = h10 -> vertex();
            VertexIter v11 = h11 -> vertex();
            // return if boundary
            if(v00 -> isBoundary() || v10 -> isBoundary()){
                return VertexIter();
            }
            // return if vertex degree is smaller than 3
            if(v01 -> degree() <= 3 || v11 -> degree() <= 3){
                return VertexIter();
            }
            // return if not manifold
            HalfedgeIter h = h20;
            while(h != h31){
                VertexIter v = h -> next() -> twin() -> vertex();
                HalfedgeIter h_ = h40;
                while(h_ -> next() != h41){
                    VertexIter v_ = h_ -> next() -> twin() -> vertex();
                    if(v == v_){
                        return VertexIter();
                    }
                    h_ = h_ -> next() -> twin();
                }
                h = h -> next() -> twin();
            }

            // Set neighbors
            // vertex
            v00 -> setNeighbors(h41);
            v01 -> setNeighbors(h40);
            v11 -> setNeighbors(h20);
            // face
            h40 -> edge() -> setNeighbors(h40);
            h41 -> edge() -> setNeighbors(h41);
            // half edge
            h20 -> setNeighbors(h20 -> next(), h41, v11, h41 -> edge(), h20 -> face());
            h21 -> setNeighbors(h21 -> next(), h40, v00, h40 -> edge(), h21 -> face());
            h40 -> setNeighbors(h40 -> next(), h21, v01, h40 -> edge(), h40 -> face());
            h41 -> setNeighbors(h41 -> next(), h20, v00, h41 -> edge(), h41 -> face());
            h = h20;
            while(h != h31){
                h -> next() -> setNeighbors(h -> next() -> next(), h -> next() -> twin(), v00, h -> next() -> edge(), h -> next() -> face());
                h = h -> next() -> twin();
            }
            // new vertex position
            v00 -> position = (v00 -> position + v10 -> position) / 2;

            // delete elements
            deleteHalfedge(h0);
            deleteHalfedge(h1);
            deleteHalfedge(h01);
            deleteHalfedge(h10);
            deleteHalfedge(h00);
            deleteHalfedge(h11);
            deleteEdge(e);
            deleteEdge(e0);
            deleteEdge(e1);
            deleteFace(f0);
            deleteFace(f1);
            deleteVertex(v10);

            return v00;
   }

   EdgeIter HalfedgeMesh::flipEdge( EdgeIter e0 )
   {
      // TODO This method should flip the given edge and return an iterator to the flipped edge.

            // assign existed elements
            HalfedgeIter h0 = e0 -> halfedge();
            HalfedgeIter h1 = h0 -> twin();
            // return if boundary
            if(h0 -> isBoundary() || h1 -> isBoundary()){
                return EdgeIter();
            }
            HalfedgeIter h00 = h0 -> next();
            HalfedgeIter h01 = h00 -> next();
            HalfedgeIter h10 = h1 -> next();
            HalfedgeIter h11 = h10 -> next();
            FaceIter f0 = h0 -> face();
            FaceIter f1 = h1 -> face();
            VertexIter v00 = h00 -> vertex();
            VertexIter v01 = h01 -> vertex();
            VertexIter v10 = h10 -> vertex();
            VertexIter v11 = h11 -> vertex();
            HalfedgeIter h = h01 -> twin() -> next();
            // return if not manifold
            if(v01 == v11){
                return EdgeIter();
            }
            while(h != h01){
                if(h -> next() -> vertex() == v11){
                    return EdgeIter();
                }
                h = h -> twin() -> next();
            }

            // Set neighbors
            h0 -> setNeighbors(h11, h1, v01, e0, f0);
            h1 -> setNeighbors(h01, h0, v11, e0, f1);
            h00 -> setNeighbors(h0, h00 -> twin(), v00, h00 -> edge(), f0);
            h01 -> setNeighbors(h10, h01 -> twin(), v01, h01 -> edge(), f1);
            h10 -> setNeighbors(h1, h10 -> twin(), v10, h10 -> edge(), f1);
            h11 -> setNeighbors(h00, h11 -> twin(), v11, h11 -> edge(), f0);
            if(f0 -> halfedge() == h01){
                f0 -> setNeighbors(h11);
            }
            if(f1 -> halfedge() == h11){
                f1 -> setNeighbors(h01);
            }
            if(v00 -> halfedge() == h1){
                v00 -> setNeighbors(h00);
            }
            if(v10 -> halfedge() == h0){
                v10 -> setNeighbors(h10);
            }

			return e0;
   }

   void MeshResampler::upsample( HalfedgeMesh& mesh )
   // This routine should increase the number of triangles in the mesh using Loop subdivision.
   {
      // Each vertex and edge of the original surface can be associated with a vertex in the new (subdivided) surface.
      // Therefore, our strategy for computing the subdivided vertex locations is to *first* compute the new positions
      // using the connectity of the original (coarse) mesh; navigating this mesh will be much easier than navigating
      // the new subdivided (fine) mesh, which has more elements to traverse.  We will then assign vertex positions in
      // the new mesh based on the values we computed for the original mesh.


      // TODO Compute new positions for all the vertices in the input mesh, using the Loop subdivision rule,
      // TODO and store them in Vertex::newPosition. At this point, we also want to mark each vertex as being
      // TODO a vertex of the original mesh.
      for( VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++ ){
          double n = v -> degree();
          double u = n == 3 ? (3.0 / 16) : (3.0 / 8 / n);
          Vector3D position = (1 - n * u) * (v -> position);
          HalfedgeIter h0 = v -> halfedge();
          HalfedgeIter h = h0;
          do{
              position += u * (h -> twin() -> vertex() -> position);
              h = h -> twin() -> next();
          }
          while(h != h0);
          v -> newPosition = position;
          v -> isNew = false;
      }

      // TODO Next, compute the updated vertex positions associated with edges, and store it in Edge::newPosition.
      for( EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++ ){
          e -> newPosition =
            (e -> halfedge() -> vertex() -> position) * 3 / 8 +
            (e -> halfedge() -> twin() -> vertex() -> position) * 3 / 8 +
            (e -> halfedge() -> next() -> next() -> vertex() -> position) / 8 +
            (e -> halfedge() -> twin() -> next() -> next() -> vertex() -> position) / 8;
          e -> isNew = false;
      }

      // TODO Next, we're going to split every edge in the mesh, in any order.  For future
      // TODO reference, we're also going to store some information about which subdivided
      // TODO edges come from splitting an edge in the original mesh, and which edges are new,
      // TODO by setting the flat Edge::isNew.  Note that in this loop, we only want to iterate
      // TODO over edges of the original mesh---otherwise, we'll end up splitting edges that we
      // TODO just split (and the loop will never end!)
      EdgeIter e = mesh.edgesBegin();
      EdgeIter endEdge = mesh.edgesEnd();
      while(e != endEdge)
      {
          EdgeIter nextEdge = e;
          nextEdge++;
          if(!e -> isNew && !e -> halfedge() -> vertex() -> isNew && !e -> halfedge() -> twin() -> vertex() -> isNew){
              VertexIter v = mesh.splitEdge( e );
              v -> position = e -> newPosition;
          }
          e = nextEdge;
      }

      // TODO Now flip any new edge that connects an old and new vertex.
      for( EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++ ){
          if(e -> isNew && (e -> halfedge() -> vertex() -> isNew ^ e -> halfedge() -> twin() -> vertex() -> isNew)){
              mesh.flipEdge( e );
          }
      }

      // TODO Finally, copy the new vertex positions into final Vertex::position.
      for( VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++ ){
          if(!v -> isNew){
              v -> position = v -> newPosition;
          }
      }
   }

   // Given an edge, the constructor for EdgeRecord finds the
   // optimal point associated with the edge's current quadric,
   // and assigns this edge a cost based on how much quadric
   // error is observed at this optimal point.
   EdgeRecord::EdgeRecord( EdgeIter& _edge )
   : edge( _edge )
   {
      // TODO Compute the combined quadric from the edge endpoints.
      Matrix4x4 quadric = _edge -> halfedge() -> vertex() -> quadric + _edge -> halfedge() -> twin() -> vertex() -> quadric;
      //cout << quadric << endl;

      // TODO Build the 3x3 linear system whose solution minimizes
      // the quadric error associated with these two endpoints.
      double data[9] = {quadric(0,0), quadric(0,1),  quadric(0,2), quadric(1,0), quadric(1,1), quadric(1,2), quadric(2,0), quadric(2,1), quadric(2,2)};
      Matrix3x3 A(data);
      Vector3D b(-quadric(0,3), -quadric(1,3), -quadric(2,3));

      // TODO Use this system to solve for the optimal position, and
      // TODO store it in EdgeRecord::optimalPoint.
      Vector3D x3D = A.inv() * b;
      optimalPoint = x3D;

      // TODO Also store the cost associated with collapsing this edge
      // TODO in EdgeRecord::Cost.
      Vector4D x4D(x3D.x, x3D.y, x3D.z, 1);
      double cost = dot(x4D, quadric * x4D);
      score = cost;

   }

   void MeshResampler::downsample( HalfedgeMesh& mesh )
   {
      // TODO Compute initial quadrics for each face by simply writing the plane
      // equation for the face in homogeneous coordinates.  These quadrics should
      // be stored in Face::quadric
      for( FaceIter f = mesh.facesBegin(); f != mesh.facesEnd(); f++ ){
          Vector3D normal = f -> normal();
          Vector3D p = f -> halfedge() -> vertex() -> position;
          double d = -dot(normal, p);
          Vector4D normal_homo = Vector4D(normal.x, normal.y, normal.z, d);
          f -> quadric = outer(normal_homo, normal_homo);
      }

      // TODO Compute an initial quadric for each vertex as the sum of the quadrics
      // associated with the incident faces, storing it in Vertex::quadric
      for( VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++ ){
          v -> quadric = Matrix4x4();
          HalfedgeIter h0 = v -> halfedge();
          HalfedgeIter h = h0;
          do{
              if (!h -> face() -> isBoundary()){
                  v -> quadric += h -> face() -> quadric;
              }
              h = h -> twin() -> next();
          }
          while(h != h0);
      }

      // TODO Build a priority queue of edges according to their quadric error cost,
      // TODO i.e., by building an EdgeRecord for each edge and sticking it in the queue.
      MutablePriorityQueue<EdgeRecord> queue;
      for( EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++ ){
          EdgeRecord record = EdgeRecord(e);
          e -> record = record;
          queue.insert( record );
      }

      // TODO Until we reach the target edge budget, collapse the best edge.  Remember
      // TODO to remove from the queue any edge that touches the collapsing edge BEFORE
      // TODO it gets collapsed, and add back into the queue any edge touching the collapsed
      // TODO vertex AFTER it's been collapsed.  Also remember to assign a quadric to the
      // TODO collapsed vertex, and to pop the collapsed edge off the top of the queue.
      Size num_face = mesh.nFaces();
      while(mesh.nFaces() > num_face / 4){
          // get cheapest edge
          EdgeRecord record = queue.top();
          // remove the record
          queue.pop();
          // new quadric
          EdgeIter e = record.edge;
          HalfedgeIter h0 = e -> halfedge();
          HalfedgeIter h1 = h0 -> twin();
          Matrix4x4 quadric = h0 -> vertex() -> quadric + h1 -> vertex() -> quadric;
          // remove records of corresponding edges
          HalfedgeIter h = h0;
          do{
              queue.remove(h -> edge() -> record);
              h = h -> twin() -> next();
          }
          while(h != h0);
          h = h1;
          do{
              queue.remove(h -> edge() -> record);
              h = h -> twin() -> next();
          }
          while(h != h1);
          // collapse
          VertexIter v = mesh.collapseEdge(e);
          // assign new quadric and new position
          v -> quadric = quadric;
          v -> position = record.optimalPoint;
          // insert new record
          h0 = v -> halfedge();
          h = h0;
          do{
              record = EdgeRecord(h -> edge());
              h -> edge() -> record = record;
              queue.insert( record );
              h = h -> twin() -> next();
          }
          while(h != h0);
      }

   }

   void Vertex::computeCentroid( void )
   {
      // TODO Compute the average position of all neighbors of this vertex, and
      // TODO store it in Vertex::centroid.  This value will be used for resampling.
      Vector3D centroid = Vector3D();
      HalfedgeIter h = this -> halfedge();
      do{
          centroid += h -> twin() -> vertex() -> position;
          h = h -> twin() -> next();
      }
      while(h != this -> halfedge());
      this -> centroid = centroid / this -> degree();
   }

   Vector3D Vertex::normal( void ) const
   // TODO Returns an approximate unit normal at this vertex, computed by
   // TODO taking the area-weighted average of the normals of neighboring
   // TODO triangles, then normalizing.
   {
      // TODO Compute and return the area-weighted unit normal.
      Vector3D normal = Vector3D();
      HalfedgeCIter h = this -> halfedge();
      do{
          normal += h -> face() -> normal();
          h = h -> twin() -> next();
      }
      while(h != this -> halfedge());
      return normal.unit();
	}

   void MeshResampler::resample( HalfedgeMesh& mesh )
   {
      // TODO Compute the mean edge length.
      double mean = 0;
      for( EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++ ){
          mean += e -> length();
      }
      mean /= mesh.nEdges();

      // TODO Repeat the four main steps for 5 or 6 iterations
      int iter = 5;
      for(int i = 0; i < iter; i++){

          // TODO Split edges much longer than the target length (being careful about how the loop is written!)
          for( EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++ ){
              if(e -> length() > mean * 3 / 2 ){
                  mesh.splitEdge(e);
              }
          }

          // TODO Collapse edges much shorter than the target length.  Here we need to be EXTRA careful about
          // TODO advancing the loop, because many edges may have been destroyed by a collapse (which ones?)
          EdgeIter e = mesh.edgesBegin();
          EdgeIter endEdge = mesh.edgesEnd();
          while(e != endEdge)
          {
              EdgeIter nextEdge = e;
              HalfedgeIter h0 = e -> halfedge();
              HalfedgeIter h1 = h0 -> twin();
              HalfedgeIter h01 = h0 -> next() -> next();
              HalfedgeIter h10 = h1 -> next();
              EdgeIter e0 = h10 -> edge();
              EdgeIter e1 = h01 -> edge();
              nextEdge++;
              while(nextEdge == e0 || nextEdge == e1){
                  nextEdge++;
              }
              if(e -> length() < mean * 3 / 4 ){
                  //cout << elementAddress(e) << endl;
                  mesh.collapseEdge(e);
              }
              e = nextEdge;
          }

          //
          // TODO Now flip each edge if it improves vertex degree
          for( EdgeIter e = mesh.edgesBegin(); e != mesh.edgesEnd(); e++ ){
              HalfedgeIter h0 = e -> halfedge();
              HalfedgeIter h1 = h0 -> twin();
              HalfedgeIter h00 = h0 -> next();
              HalfedgeIter h01 = h00 -> next();
              HalfedgeIter h10 = h1 -> next();
              HalfedgeIter h11 = h10 -> next();
              VertexIter v00 = h00 -> vertex();
              VertexIter v01 = h01 -> vertex();
              VertexIter v10 = h10 -> vertex();
              VertexIter v11 = h11 -> vertex();
              int degree_before = abs((int)(v00 -> degree()) - 6) + abs((int)(v10 -> degree()) - 6) + abs((int)(v01 -> degree()) - 6) + abs((int)(v11 -> degree()) - 6);
              int degree_after = abs((int)(v00 -> degree()) - 7) + abs((int)(v10 -> degree()) - 7) + abs((int)(v01 -> degree()) - 5) + abs((int)(v11 -> degree()) - 5);
              if(degree_before > degree_after){
                  mesh.flipEdge(e);
              }
          }

          // TODO Finally, apply some tangential smoothing to the vertex positions
          int smoothIter = 20;
          for(int j = 0; j < smoothIter; j++){
              for( VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++ ){
                  v -> computeCentroid();
              }
              double w = 0.2;
              for( VertexIter v = mesh.verticesBegin(); v != mesh.verticesEnd(); v++ ){
                  Vector3D dir = v -> centroid - v -> position;
                  v -> position = v -> position + w * (dir - dot(v -> normal(), dir) * v -> normal());
              }
          }
      }
   }
}
