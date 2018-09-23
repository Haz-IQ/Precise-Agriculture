from django.conf.urls import url
from . import views

urlpatterns = [
    url(r'^$', views.index, name='index'),
    url(r'^contact/' , views.contact , name = 'contact'),
    url(r'^api/data/' , views.get_data, name = 'api-data'),
    url(r'^api/chart/data/' , views.ChartData.as_view() , name = 'chart-data')
    ]
