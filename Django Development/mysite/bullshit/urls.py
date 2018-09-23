from django.conf.urls import url, include
from django.views.generic import ListView, DetailView
from bullshit.models import Post

urlpatterns = [ 
                url(r'^$', ListView.as_view(
                                    queryset=Post.objects.all().order_by("-date")[:25],
                                    template_name="bullshit/bullshit.html")),
                url(r'^(?P<pk>\d+)$', DetailView.as_view(model = Post,
                                                         template_name = 'bullshit/post.html'))]
