template <HistoryDAGReference T>
TraverseValue<T>::TraverseValue(T dag, NodeId node, EdgeId edge) :
		dag_{dag}, node_{node}, edge_{edge} {}

template <HistoryDAGReference T>
NodeView<T> TraverseValue<T>::GetNode() const { return dag_.GetNode(node_); }

template <HistoryDAGReference T>
EdgeView<T> TraverseValue<T>::GetEdge() const { return dag_.GetEdge(edge_); }

template <HistoryDAGReference T>
TraverseValue<T>::operator MutableNode() const { return GetNode(); }

template <HistoryDAGReference T>
TraverseValue<T>::operator MutableEdge() const { return GetEdge(); }

template <HistoryDAGReference T>
TraverseValue<T>::operator Node() const { return GetNode(); }

template <HistoryDAGReference T>
TraverseValue<T>::operator Edge() const { return GetEdge(); }
