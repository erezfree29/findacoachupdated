class PagesController < ApplicationController
  skip_before_action :authenticate_user, only: [:create], raise: false

  def home
  end


end
