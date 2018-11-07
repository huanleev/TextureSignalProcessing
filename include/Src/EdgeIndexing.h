/*
Copyright (c) 2018, Fabian Prada and Michael Kazhdan
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer. Redistributions in binary form must reproduce
the above copyright notice, this list of conditions and the following disclaimer
in the documentation and/or other materials provided with the distribution.

Neither the name of the Johns Hopkins University nor the names of its contributors
may be used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#ifndef EDGE_INDEXING_INCLUDED
#define EDGE_INDEXING_INCLUDED

int InitializeIntraChartEdgeIndexing(std::unordered_map<unsigned long long, int> & boundaryCoarseEdgeIndex, const GridChart & gridChart, int & lastAddedEdgeIndex)
{	
	int edgesPerCell = 2;
	int pairsToAdd[4] = { 0,1,0,3 };
	// Node indexing
	//		0 ---- 1
	//		|	   |
	//		|	   |
	//		3------2

	// Edge indexing
	//		 ---0---
	//		|  
	//		1  
	//		|  

	for (int i = 0; i < gridChart.bilinearElementIndices.size(); i++){
		const BilinearElementIndex & indices = gridChart.bilinearElementIndices[i];
		for (int k = 0; k < edgesPerCell; k++) {
			int vIndices[2] = { (int)indices[ pairsToAdd[2*k] ] , (int)indices[ pairsToAdd[2*k+1] ] };
			unsigned long long edgeKey = SetMeshEdgeKey(vIndices[0], vIndices[1]);
			if (boundaryCoarseEdgeIndex.find(edgeKey) == boundaryCoarseEdgeIndex.end()) {
				boundaryCoarseEdgeIndex[edgeKey] = lastAddedEdgeIndex;
				lastAddedEdgeIndex++;
			}
			else {
				printf("ERROR: Edge already added!\n");
				return 0;
			}
		}
	}
	return 1;
}

int InitializeIntraChartEdgeIndexing(const std::vector<GridChart> & gridCharts, std::unordered_map<unsigned long long, int> &  boundaryCoarseEdgeIndex) {
	//Add edges within charts
	int lastAddedEdgeIndex = 0;
	for (int i = 0; i < gridCharts.size(); i++) {
		if (!InitializeIntraChartEdgeIndexing(boundaryCoarseEdgeIndex, gridCharts[i], lastAddedEdgeIndex)) {
			return 0;
		}
	}
	return 1;
}

int InitializeBoundaryEdgeIndexing(const SparseMatrix<double ,int> & boundaryAdjancencyMatrix, const std::vector<int> & boundaryGlobalIndex,std::unordered_map<unsigned long long, int> &  coarseEdgeIndex, std::vector<int> & boundaryEdgeToGlobalEdge, std::unordered_map<unsigned long long, int> &  boundaryEdgeIndex){
	int lastAddedCoarseEdgeIndex = (int)coarseEdgeIndex.size();
	int lastAddedBoundaryEdgeIndex = 0;
	for (int r = 0; r < boundaryAdjancencyMatrix.Rows(); r++){
		for(int c = 0; c< boundaryAdjancencyMatrix.RowSize(r); c++){
			int bIndices[2] = {r,boundaryAdjancencyMatrix[r][c].N};
			if (bIndices[0] != bIndices[1]) {
				unsigned long long bminKey = SetMeshEdgeKey(bIndices[0], bIndices[1]);
				unsigned long long bmaxKey = SetMeshEdgeKey(bIndices[1], bIndices[0]);
				if (boundaryEdgeIndex.find(bminKey) == boundaryEdgeIndex.end() && boundaryEdgeIndex.find(bmaxKey) == boundaryEdgeIndex.end()){
					int gIndices[2] = { boundaryGlobalIndex[bIndices[0]],boundaryGlobalIndex[bIndices[1]] };
					unsigned long long minKey = SetMeshEdgeKey(gIndices[0], gIndices[1]);
					unsigned long long maxKey = SetMeshEdgeKey(gIndices[1], gIndices[0]);
					int globalEdgeIndex = -1;
					if (coarseEdgeIndex.find(minKey) != coarseEdgeIndex.end()) {
						globalEdgeIndex = coarseEdgeIndex[minKey];
						boundaryEdgeIndex[bminKey] = lastAddedBoundaryEdgeIndex;
						lastAddedBoundaryEdgeIndex++;
					}
					else if (coarseEdgeIndex.find(maxKey) != coarseEdgeIndex.end()) {
						globalEdgeIndex = coarseEdgeIndex[maxKey];
						boundaryEdgeIndex[bmaxKey] = lastAddedBoundaryEdgeIndex;
						lastAddedBoundaryEdgeIndex++;
					}
					else {
						coarseEdgeIndex[minKey] = lastAddedCoarseEdgeIndex;
						globalEdgeIndex = lastAddedCoarseEdgeIndex;
						lastAddedCoarseEdgeIndex++;

						boundaryEdgeIndex[bminKey] = lastAddedBoundaryEdgeIndex;
						lastAddedBoundaryEdgeIndex++;
					}
					boundaryEdgeToGlobalEdge.push_back(globalEdgeIndex);
				}
			}
		}
	}
	return 1;
}

int InitializeFineBoundaryEdgeChartIndexing(const std::vector<int> & fineBoundaryNodeIndex, std::unordered_map<unsigned long long, int> & fineBoundaryEdgeIndex, const GridChart & gridChart, int & lastAddedEdgeIndex) {

	for (int c = 0; c < gridChart.boundaryTriangles.size(); c++) {
		for (int b = 0; b < gridChart.boundaryTriangles[c].size(); b++) {
			const int i = gridChart.boundaryTriangles[c][b].id;
			const QuadraticElementIndex & indices = gridChart.boundaryTriangles[c][b].indices;
			int fineHexIndices[6];
			for (int k = 0; k < 6; k++) fineHexIndices[k] = fineBoundaryNodeIndex[indices[k]];
			for (int k = 1; k < 6; k++) for (int l = 0; l < k; l++) {
				int vIndices[2] = { fineHexIndices[k],fineHexIndices[l] };
				if (vIndices[0] > vIndices[1]) std::swap(vIndices[0], vIndices[1]);
				unsigned long long edgeKey = SetMeshEdgeKey(vIndices[0], vIndices[1]);
				if (fineBoundaryEdgeIndex.find(edgeKey) == fineBoundaryEdgeIndex.end()) {
					fineBoundaryEdgeIndex[edgeKey] = lastAddedEdgeIndex;
					lastAddedEdgeIndex++;
				}
			}
		}
	}
	return 1;
}

int InitializeFineBoundaryEdgeIndexing(const std::vector<int> & fineBoundaryNodeIndex, std::unordered_map<unsigned long long, int> & fineBoundaryEdgeIndex, const std::vector<GridChart> & gridCharts){
	int lastAddedEdgeIndex = 0;
	for (int i = 0; i < gridCharts.size(); i++) {
		if (!InitializeFineBoundaryEdgeChartIndexing(fineBoundaryNodeIndex, fineBoundaryEdgeIndex, gridCharts[i], lastAddedEdgeIndex)) {
			return 0;
		}
	}
	return 1;
}

int InitializeBoundaryCoarseToFineBoundaryOneFormProlongation(const SparseMatrix<double, int> & boundaryCoarseToFineNodeProlongation, std::unordered_map<unsigned long long, int> & boundaryCoarseEdgeIndex, std::unordered_map<unsigned long long, int> & boundaryFineEdgeIndex, SparseMatrix<double, int> & boundaryFineToBoundaryCoarseOneFormProlongation) {

	std::vector<Eigen::Triplet<double>> coarseToFineOneFormProlongation;
	for (auto fineEdgeIter = boundaryFineEdgeIndex.begin();fineEdgeIter != boundaryFineEdgeIndex.end(); fineEdgeIter++) {
		unsigned long long fineEdgeKey = (*fineEdgeIter).first;
		int fineEdgeId = (*fineEdgeIter).second;

		unsigned long fineEdgeCorners[2];
		GetMeshEdgeIndices(fineEdgeKey, fineEdgeCorners[0], fineEdgeCorners[1]);

		for (int k = 0; k < boundaryCoarseToFineNodeProlongation.RowSize(fineEdgeCorners[0]); k++) {

			int coarseIndex1 = boundaryCoarseToFineNodeProlongation[fineEdgeCorners[0]][k].N;
			double coarseValue1 = boundaryCoarseToFineNodeProlongation[fineEdgeCorners[0]][k].Value;

			for (int l = 0; l < boundaryCoarseToFineNodeProlongation.RowSize(fineEdgeCorners[1]); l++) {

				int coarseIndex2 = boundaryCoarseToFineNodeProlongation[fineEdgeCorners[1]][l].N;
				double coarseValue2 = boundaryCoarseToFineNodeProlongation[fineEdgeCorners[1]][l].Value;
				
				if (coarseIndex1 != coarseIndex2){
					bool foundEdge = false;
					unsigned long long coarseEdgeKey = SetMeshEdgeKey(coarseIndex1, coarseIndex2);
					auto coarseEdgePtr = boundaryCoarseEdgeIndex.find(coarseEdgeKey);
					if (coarseEdgePtr != boundaryCoarseEdgeIndex.end()) {
						foundEdge = true;
						int coarseEdgeId = coarseEdgePtr->second;
						coarseToFineOneFormProlongation.push_back(Eigen::Triplet<double>(fineEdgeId, coarseEdgeId, coarseValue1 * coarseValue2));
					}
					else{
						coarseEdgeKey = SetMeshEdgeKey(coarseIndex2, coarseIndex1);
						coarseEdgePtr = boundaryCoarseEdgeIndex.find(coarseEdgeKey);
						if (coarseEdgePtr != boundaryCoarseEdgeIndex.end()) {
							foundEdge = true;
							int coarseEdgeId = coarseEdgePtr->second;
							coarseToFineOneFormProlongation.push_back(Eigen::Triplet<double>(fineEdgeId, coarseEdgeId, -coarseValue1 *coarseValue2));
						}
					}
					if (!foundEdge){
						printf("ERROR : Edge (%d,%d) not found.\n", coarseIndex1, coarseIndex2);
						return 0;
					}
				}
			}
		}
	}
	
	int numCoarseOneForms = (int)boundaryCoarseEdgeIndex.size();
	int numFineOneForms = (int)boundaryFineEdgeIndex.size();

	boundaryFineToBoundaryCoarseOneFormProlongation = SetSparseMatrix(coarseToFineOneFormProlongation, numFineOneForms, numCoarseOneForms, false);

	return 1;
}
#endif // EDGE_INDEXING_INCLUDED